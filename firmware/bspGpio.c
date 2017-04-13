/*
 * bspGpio.c
 *
 *  Created on: Jan 19, 2017
 *      Author: shapa
 */

#include <stdlib.h>

#include "bsp.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_syscfg.h"

typedef struct {
	const GPIO_TypeDef *port;
	const GPIO_InitTypeDef setting;
} BspGpioConfig_t;

static const BspGpioConfig_t s_gpioConfig[] = {
	[BSP_Pin_VFD_F] = { GPIOF, { GPIO_Pin_0, GPIO_Mode_OUT,
			GPIO_Speed_Level_1, GPIO_OType_PP,  GPIO_PuPd_NOPULL} },
	[BSP_Pin_VFD_B] = { GPIOF, { GPIO_Pin_1, GPIO_Mode_OUT,
			GPIO_Speed_Level_1, GPIO_OType_PP,  GPIO_PuPd_NOPULL} },
	[BSP_Pin_VFD_A] = { GPIOA, { GPIO_Pin_0, GPIO_Mode_OUT,
			GPIO_Speed_Level_1, GPIO_OType_PP,  GPIO_PuPd_NOPULL} },

	[BSP_Pin_74HC595_DS] = { GPIOA, { GPIO_Pin_1, GPIO_Mode_OUT,
			GPIO_Speed_Level_3, GPIO_OType_PP,  GPIO_PuPd_NOPULL} },
	[BSP_Pin_74HC595_NOE] = { GPIOA, { GPIO_Pin_2, GPIO_Mode_OUT,
			GPIO_Speed_Level_3, GPIO_OType_PP,  GPIO_PuPd_NOPULL} },
	[BSP_Pin_74HC595_Store] = { GPIOA, { GPIO_Pin_3, GPIO_Mode_OUT,
			GPIO_Speed_Level_3, GPIO_OType_PP,  GPIO_PuPd_NOPULL} },
	[BSP_Pin_74HC595_Shift] = { GPIOA, { GPIO_Pin_4, GPIO_Mode_OUT,
			GPIO_Speed_Level_3, GPIO_OType_PP,  GPIO_PuPd_NOPULL} },

	[BSP_Pin_KEY_1] = { GPIOA, { GPIO_Pin_5, GPIO_Mode_IN,
			GPIO_Speed_Level_1, GPIO_OType_PP,  GPIO_PuPd_UP} },
	[BSP_Pin_KEY_2] = { GPIOA, { GPIO_Pin_6, GPIO_Mode_IN,
			GPIO_Speed_Level_1, GPIO_OType_PP,  GPIO_PuPd_UP} },

	[BSP_Pin_VBAT] = { GPIOA, { GPIO_Pin_7, GPIO_Mode_AN,
			GPIO_Speed_Level_1, GPIO_OType_PP,  GPIO_PuPd_NOPULL} },

	[BSP_Pin_POWER_SWITCH] = { GPIOA, { GPIO_Pin_9, GPIO_Mode_OUT,
			GPIO_Speed_Level_1, GPIO_OType_PP,  GPIO_PuPd_NOPULL} },
	[BSP_Pin_VFD_G4] = { GPIOA, { GPIO_Pin_10, GPIO_Mode_OUT,
			GPIO_Speed_Level_1, GPIO_OType_PP,  GPIO_PuPd_NOPULL} },
};

static void initGPIO_NVIC(void);

void BSP_InitGpio(void) {
	static const size_t size = sizeof(s_gpioConfig)/sizeof(*s_gpioConfig);
	for (size_t i = 0; i < size; i++)
		GPIO_Init((GPIO_TypeDef*)s_gpioConfig[i].port, (GPIO_InitTypeDef*)&s_gpioConfig[i].setting);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource5);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource6);

	initGPIO_NVIC();

	BSP_SetPinVal(BSP_Pin_POWER_SWITCH, false);
	BSP_SetPinVal(BSP_Pin_74HC595_NOE, true);
}

void BSP_SetPinVal(const BSP_Pin_t pin, const _Bool state) {
	if (pin > BSP_Pin_Last)
		return;
	BitAction act = state ? Bit_SET : Bit_RESET;
	GPIO_WriteBit((GPIO_TypeDef*)s_gpioConfig[pin].port, s_gpioConfig[pin].setting.GPIO_Pin, act);
}

_Bool BSP_GetPinVal(const BSP_Pin_t pin) {
	if (pin > BSP_Pin_Last)
		return false;
	return GPIO_ReadInputDataBit((GPIO_TypeDef*)s_gpioConfig[pin].port, s_gpioConfig[pin].setting.GPIO_Pin);
}

static void initGPIO_NVIC(void) {
	NVIC_InitTypeDef nvic = {
			EXTI4_15_IRQn,
			0,
			ENABLE
	};
	NVIC_Init(&nvic);
}

