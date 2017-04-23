/*
 * buttons.c
 *
 *  Created on: Sep 25, 2016
 *      Author: shapa
 */

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "stm32f0xx_gpio.h"
#include "stm32f0xx_exti.h"
#include "system.h"
#include "bsp.h"
#include "systemTimer.h"
#include "timers.h"
#include "Queue.h"

#define KEY_DEBOUNCE_TOUT 50
#define KEY_FIRST_REPEAT_TOUT 500
#define KEY_REPEAT_TOUT 10

void EXTI4_15_IRQHandler(void);

static struct {
	uint32_t timerId;
	_Bool state;
	const _Bool isRepeatable;
} s_buttons[BUTTON_LAST]= {
		[BUTTON_UP] 	= { .isRepeatable = true },
		[BUTTON_DOWN] 	= { .isRepeatable = true },
};

static void onButtonIsr(Buttons_t button, _Bool state);
static void onDebounceTimer(uint32_t id, void *data);
static void onRepeatTimer(uint32_t id, void *data);

void EXTI4_15_IRQHandler(void) {
	if (EXTI_GetFlagStatus(EXTI_Line5)) {
		_Bool state = !BSP_GetPinVal(BSP_Pin_KEY_DOWN);
		onButtonIsr(BUTTON_UP, state);
		EXTI_ClearFlag(EXTI_Line5);
	}
	if (EXTI_GetFlagStatus(EXTI_Line9)) {
		_Bool state = !BSP_GetPinVal(BSP_Pin_KEY_UP);
		onButtonIsr(BUTTON_DOWN, state);
		EXTI_ClearFlag(EXTI_Line9);
	}
}

static void onButtonIsr(Buttons_t button, _Bool state) {

	if (button < BUTTON_LAST) {
		Timer_delete(s_buttons[button].timerId);
		s_buttons[button].timerId = INVALID_HANDLE;
		if (state ^ s_buttons[button].state) {
			s_buttons[button].state = state;
			if (state) {
				//press
				s_buttons[button].timerId = Timer_newArmed(KEY_DEBOUNCE_TOUT, false, onDebounceTimer, (void*)button);
			} else {
				uint32_t val = ACTION_RELESE | button;
				EventQueue_Push(EVENT_KEY, (void*)val, NULL);
			}
		}
	}
}

static void onDebounceTimer(uint32_t id, void *data) {
	Buttons_t button = (Buttons_t)data;
	if ((button < BUTTON_LAST) && (s_buttons[button].timerId == id)) {

		uint32_t val = ACTION_PRESS | button;
		EventQueue_Push(EVENT_KEY, (void*)val, NULL);
		s_buttons[button].timerId = INVALID_HANDLE;
		if (s_buttons[button].isRepeatable) {
			s_buttons[button].timerId = Timer_newArmed(KEY_FIRST_REPEAT_TOUT, true, onRepeatTimer, (void*)button);
		}
	}
}

static void onRepeatTimer(uint32_t id, void *data) {
	Buttons_t button = (Buttons_t)data;
	Timer_rearmTimeout(id, KEY_REPEAT_TOUT);
	if ((button < BUTTON_LAST) && (s_buttons[button].timerId == id)) {
		uint32_t val = ACTION_REPEAT | button;
		EventQueue_Push(EVENT_KEY, (void*)val, NULL);
	}
}
