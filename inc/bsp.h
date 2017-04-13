/*
 * bsp.h
 *
 *  Created on: Jan 18, 2017
 *      Author: pavelgamov
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#define BSP_TICKS_PER_SECOND 1000
#define MINUTE_TICKS (BSP_TICKS_PER_SECOND*60)

typedef enum {
	BSP_Pin_VFD_F,
	BSP_Pin_VFD_B,
	BSP_Pin_VFD_A,

	BSP_Pin_74HC595_DS,
	BSP_Pin_74HC595_NOE,
	BSP_Pin_74HC595_ST,
	BSP_Pin_74HC595_SH,

	BSP_Pin_KEY_1,
	BSP_Pin_KEY_2,

	BSP_Pin_VBAT,

	BSP_Pin_POWER_SWITCH,
	BSP_Pin_VFD_G4,

	BSP_Pin_Last,
} BSP_Pin_t;


_Bool BSP_Init(void);
void BSP_InitGpio(void);

void BSP_FeedWatchdog(void);

void BSP_SetPinVal(const BSP_Pin_t pin, const _Bool state);
_Bool BSP_GetPinVal(const BSP_Pin_t pin);

#ifdef __cplusplus
}
#endif
