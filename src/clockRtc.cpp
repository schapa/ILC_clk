/*
 * clock.cpp
 *
 *  Created on: Apr 14, 2017
 *      Author: shapa
 */


#include <stdio.h>
#include <stdlib.h>

#include "bsp.h"
#include "timers.h"
#include "clock.h"


#include "stm32f0xx_rcc.h"
#include "stm32f0xx_pwr.h"
#include "stm32f0xx_rtc.h"

void Clock_rtcInit(void) {

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	RCC_LSICmd(ENABLE);
	while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
	PWR_BackupAccessCmd(ENABLE);
	RCC_BackupResetCmd(ENABLE);
	RCC_BackupResetCmd(DISABLE);
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
	RCC_RTCCLKCmd(ENABLE);
	RTC_WaitForSynchro();

	RTC_InitTypeDef rtc = {
			RTC_HourFormat_24,
			0x63,
			0x18F,
	};
    RTC_Init(&rtc);
    RTC_TimeTypeDef time;
    RTC_GetTime(RTC_Format_BIN, &time);
}

void Clock_rtcSetTime(const uint8_t &hours, const uint8_t &minutes) {

    RTC_TimeTypeDef time;
    RTC_GetTime(RTC_Format_BIN, &time);
    time.RTC_Hours = hours;
    time.RTC_Minutes = minutes;
    time.RTC_Seconds = 0;
    RTC_SetTime(RTC_Format_BIN, &time);
}

void Clock_rtcGetTime(uint8_t &hours, uint8_t &minutes) {

    RTC_TimeTypeDef time;
    RTC_GetTime(RTC_Format_BIN, &time);
    hours = time.RTC_Hours;
    minutes = time.RTC_Minutes;
}
