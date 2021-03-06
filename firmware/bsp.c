/*
 * bsp.c
 *
 *  Created on: May 18, 2016
 *      Author: shapa
 */

#include "bsp.h"
#include "system.h"
#include "systemTimer.h"
#include "Queue.h"
#include "timers.h"

#include "stm32f0xx_rcc.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_adc.h"
#include "stm32f0xx_iwdg.h"
#include "stm32f0xx_dbgmcu.h"

#include <stddef.h>

#define ADC_TIMEOUT (1*BSP_TICKS_PER_SECOND)

static void initialize_RCC(void);
static void initWdt(void);
static void initADC(void);
static void initADC_NVIC(void);

static void onAdcTimeout(uint32_t id, void *data);

static uint16_t adcToVoltsVbatt(const uint16_t adc);
static uint32_t s_adcTimerId = INVALID_HANDLE;

_Bool BSP_Init(void) {
	initialize_RCC();
	initWdt();
	BSP_InitGpio();
	System_init(NULL);
	System_setStatus(INFORM_IDLE);
	initADC();

	return true;
}

void BSP_FeedWatchdog(void) {
	IWDG_ReloadCounter();
}


static void initialize_RCC(void) {

	RCC_HSEConfig(RCC_HSE_OFF);
	RCC_ADCCLKConfig(RCC_ADCCLK_PCLK_Div4);

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	GPIO_DeInit(GPIOA);
	GPIO_DeInit(GPIOB);
	GPIO_DeInit(GPIOF);
}

static void initWdt(void) {
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_32);
	IWDG_SetReload(0x0FFF);
	IWDG_ReloadCounter();
	IWDG_Enable();
	DBGMCU_APB1PeriphConfig(DBGMCU_IWDG_STOP, ENABLE);
}

static void initADC(void) {
	const static ADC_InitTypeDef iface = {
		ADC_Resolution_12b,
		DISABLE,
		ADC_ExternalTrigConvEdge_None,
		0,
		ADC_DataAlign_Right,
		ADC_ScanDirection_Upward
	};
	s_adcTimerId = Timer_new(ADC_TIMEOUT, true, onAdcTimeout, NULL);

	ADC_DeInit(ADC1);
	ADC_Init(ADC1, (ADC_InitTypeDef*)&iface);
	ADC_ClockModeConfig(ADC1, ADC_ClockMode_SynClkDiv4);
	ADC_ChannelConfig(ADC1, ADC_Channel_0, ADC_SampleTime_239_5Cycles);

	initADC_NVIC();
	ADC_ContinuousModeCmd(ADC1, DISABLE);
	ADC_DiscModeCmd(ADC1, ENABLE);

	ADC_ITConfig(ADC1, ADC_IT_ADRDY, ENABLE);
	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

	ADC_GetCalibrationFactor(ADC1);
	ADC_Cmd(ADC1, ENABLE);

}

static void initADC_NVIC(void) {
	NVIC_InitTypeDef nvic = {
			ADC1_IRQn,
			0,
			ENABLE
	};
	NVIC_Init(&nvic);
}

static void onAdcTimeout(uint32_t id, void *data) {
	Timer_disarm(id);
	ADC_ChannelConfig(ADC1, ADC_Channel_0, ADC_SampleTime_239_5Cycles);
	ADC_StartOfConversion(ADC1);
}

void ADC1_IRQHandler(void) {
	if (ADC_GetITStatus(ADC1, ADC_IT_EOC)) {
		ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
		uint32_t val = ADC_GetConversionValue(ADC1);
		val = adcToVoltsVbatt(val);
		EventQueue_Push(EVENT_ADC, (void*)val, NULL);
		Timer_rearm(s_adcTimerId);

	}
	if (ADC_GetITStatus(ADC1, ADC_IT_ADRDY)) {
		ADC_ClearITPendingBit(ADC1, ADC_IT_ADRDY);
		Timer_rearm(s_adcTimerId);
	}
}

static uint16_t adcToVoltsVbatt(const uint16_t adc) {
	const uint32_t adcVolts = (adc*3300)/4096;
	static const uint8_t rHi = 100;
	static const uint8_t rLo = 100;

	return (uint16_t)adcVolts*(rHi+rLo)/rLo;
}
