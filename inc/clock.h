/*
 * clock.h
 *
 *  Created on: Apr 13, 2017
 *      Author: shapa
 */

#pragma once

void Clock_init(void);

void Clock_setTime(const uint8_t &h, const uint8_t &m);

void Clock_rtcInit(void);
void Clock_rtcSetTime(const uint8_t &hours, const uint8_t &minutes);
void Clock_rtcGetTime(uint8_t &hours, uint8_t &minutes);

