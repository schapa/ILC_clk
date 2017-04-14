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


static void writeShiftReg(const uint8_t &val);
static void onScanTimer(uint32_t id, void *data);
static void onBlinkTimer(uint32_t id, void *data);

void Clock_init(void) {
	Clock_rtcInit();
	s_display.scanTimerId = Timer_newArmed(BSP_TICKS_PER_SECOND/100, true, onScanTimer, NULL);
	s_display.blinkTimerId = Timer_newArmed(BSP_TICKS_PER_SECOND/2, true, onBlinkTimer, NULL);
}

void Clock_setTime(const uint8_t &h, const uint8_t &m) {
	s_display.values[0] = m % 10;
	s_display.values[1] = (m/10) % 10;
	s_display.values[3] = h % 10;
	s_display.values[4] = (h/10) % 10;
}

static void onScanTimer(uint32_t id, void *data) {
	static const uint8_t maskValReg[] = {
			0x54, 0x10, 0xC4, 0x94, 0x04,
			0x94, 0xC4, 0x10, 0xC4, 0x94
	};
	static const uint8_t maskValPin[] = {
			0x07, 0x02, 0x03, 0x03, 0x06,
			0x05, 0x05, 0x07, 0x07, 0x07
	};
	static const uint8_t maskGridReg[] = {
			0x01, 0x20, 0x08, 0x02, 0x00
	};
	BSP_SetPinVal(BSP_Pin_74HC595_NOE, false);

	if (++s_display.currentPos == sizeof(s_display.values))
		s_display.currentPos = 0;
	const uint8_t regVal = maskValReg[s_display.values[s_display.currentPos]]
								| maskGridReg[s_display.currentPos];
	const uint8_t pinVal = maskValPin[s_display.values[s_display.currentPos]];

	writeShiftReg(regVal);
	BSP_SetPinVal(BSP_Pin_VFD_A, pinVal & 1);
	BSP_SetPinVal(BSP_Pin_VFD_B, pinVal & 2);
	BSP_SetPinVal(BSP_Pin_VFD_F, pinVal & 4);
	BSP_SetPinVal(BSP_Pin_VFD_G4, (s_display.currentPos == 4));
	BSP_SetPinVal(BSP_Pin_74HC595_NOE, true);
}

static void onBlinkTimer(uint32_t id, void *data) {
	s_display.values[2] = s_display.values[2] ? 0x00 : 0x84;
}

#include "stm32f0xx.h"

static void writeShiftReg(const uint8_t &val) {
	for (size_t i = 0; i < 8; i++) {
		BSP_SetPinVal(BSP_Pin_74HC595_DS, val & (0x80>>i));
		BSP_SetPinVal(BSP_Pin_74HC595_Shift, false);
		BSP_SetPinVal(BSP_Pin_74HC595_Shift, true);
	}
	BSP_SetPinVal(BSP_Pin_74HC595_Store, true);
	BSP_SetPinVal(BSP_Pin_74HC595_Store, false);
}
