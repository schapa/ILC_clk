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
	BSP_Pin_74HC595_Store,
	BSP_Pin_74HC595_Shift,

	BSP_Pin_KEY_1,
	BSP_Pin_KEY_2,

	BSP_Pin_VBAT,

	BSP_Pin_POWER_SWITCH,
	BSP_Pin_VFD_G4,

	BSP_Pin_Last,
} BSP_Pin_t;

typedef enum {
	BUTTON_UP, // BSP_Pin_Key_1
	BUTTON_DOWN, // BSP_Pin_Key_4
	BUTTON_LAST
} Buttons_t;

typedef enum {
	ACTION_PRESS  = 0x10,
	ACTION_REPEAT = 0x20,
	ACTION_RELESE = 0x40,
	ACTION_LAST
} Action_t;


_Bool BSP_Init(void);
void BSP_InitGpio(void);

void BSP_FeedWatchdog(void);

void BSP_SetPinVal(const BSP_Pin_t pin, const _Bool state);
_Bool BSP_GetPinVal(const BSP_Pin_t pin);

#ifdef __cplusplus
}
#endif
