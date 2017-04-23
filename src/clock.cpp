/*

 * shiftReg.cpp
 *
 *  Created on: Apr 13, 2017
 *      Author: shapa
 */

#include <stdio.h>
#include <stdlib.h>

#include "bsp.h"
#include "timers.h"
#include "clock.h"

static struct {
	uint32_t scanTimerId;
	uint32_t blinkTimerId;
	uint8_t values[5];
	uint8_t currentPos;
} s_display;


static void writeShiftReg(const uint16_t &val);
static void onScanTimer(uint32_t id, void *data);
static void onBlinkTimer(uint32_t id, void *data);

void Clock_init(void) {
	Clock_rtcInit();
	s_display.scanTimerId = Timer_newArmed(BSP_TICKS_PER_SECOND/500, true, onScanTimer, NULL);
	s_display.blinkTimerId = Timer_newArmed(BSP_TICKS_PER_SECOND/2, true, onBlinkTimer, NULL);
}

void Clock_setTime(const uint8_t &h, const uint8_t &m) {
	s_display.values[0] = m % 10;
	s_display.values[1] = (m/10) % 10;
	s_display.values[3] = h % 10;
	s_display.values[4] = (h/10) % 10;
}

static void onScanTimer(uint32_t id, void *data) {
	static const uint16_t maskPosition[] = {
		0x0002, 0x0020, 0x0080, 0x0400, 0x2000,
	};
	static const uint16_t maskDigit[] = {
		0x095C, 0x0110, 0x1854, 0x1154, 0x1118,
		0x114C, 0x194C, 0x011C, 0x195C, 0x115C
	};
	if (++s_display.currentPos == sizeof(s_display.values))
		s_display.currentPos = 0;
	const uint16_t value = (s_display.currentPos == 2) ?
			maskPosition[s_display.currentPos] | s_display.values[2]*0x1040 :
			maskDigit[s_display.values[s_display.currentPos]] | maskPosition[s_display.currentPos];
	BSP_SetPinVal(BSP_Pin_74HC595_NOE, true);
	writeShiftReg(~value);
	BSP_SetPinVal(BSP_Pin_74HC595_NOE, false);
}

static void onBlinkTimer(uint32_t id, void *data) {
	s_display.values[2] = !s_display.values[2];
}

#include "stm32f0xx.h"

static void writeShiftReg(const uint16_t &val) {
	for (size_t i = 0; i < 16; i++) {
		BSP_SetPinVal(BSP_Pin_74HC595_DS, val & (0x8000>>i));
		BSP_SetPinVal(BSP_Pin_74HC595_Shift, false);
		BSP_SetPinVal(BSP_Pin_74HC595_Shift, true);
	}
}
