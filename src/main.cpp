
#include <stdio.h>
#include <stdlib.h>

#include "bsp.h"
#include "Queue.h"
#include "timers.h"
#include "systemTimer.h"
#include "clock.h"

#define TIME_MAX (24*60)


static void onTimerPush(uint32_t id);

typedef void* (*state_f)(const Event_t&);

static void* state_On(const Event_t &event);
static void* state_Off(const Event_t &event);
static void proccessKey(const uint32_t &key);

int main(int argc, char* argv[]) {

	Timer_init(onTimerPush);
	BSP_Init();
	Clock_init();
	Clock_setTime(12, 34);
	state_f state = state_On;
	BSP_SetPinVal(BSP_Pin_POWER_SWITCH, true);
	while (true) {
		Event_t event;
		EventQueue_Pend(&event);
		BSP_FeedWatchdog();
		if (EVENT_TIMCALL == event.type) {
			Timer_onTimerCb(reinterpret_cast<uint32_t>(event.data));
		} else
			state = reinterpret_cast<state_f>(state(event));
		EventQueue_Dispose(&event);
	}
	return 0;
}

static void onTimerPush(uint32_t id) {
	EventQueue_Push(EVENT_TIMCALL, (void*)id, NULL);
}

static void* state_On(const Event_t &event) {
	state_f state = state_On;
	uint32_t intVal = (uint32_t)event.data;
	switch (event.type) {
		case EVENT_SYSTICK: {
				uint8_t h = 0;
				uint8_t m = 0;
				Clock_rtcGetTime(h, m);
				Clock_setTime(h, m);
			}
			break;
		case EVENT_ADC:
			if (intVal < 36000) {
//				Clock_setTime(intVal/1000,(intVal/10)%100);
//				state = state_Off;
//				BSP_SetPinVal(BSP_Pin_POWER_SWITCH, true);
//				BSP_SetPinVal(BSP_Pin_74HC595_NOE, false);
			}
			break;
		case EVENT_KEY: {
			proccessKey(intVal);
			break;
		}
		default:
			break;
	}
	return reinterpret_cast<void*>(state);
}

static void* state_Off(const Event_t &event) {
	state_f state = state_Off;
	switch (event.type) {
		case EVENT_KEY: {
			const Action_t action = (Action_t)((int)event.data & 0xF0);
			if (action == ACTION_RELESE) {
				state = state_On;
			} else {
				BSP_SetPinVal(BSP_Pin_POWER_SWITCH, false);
				BSP_SetPinVal(BSP_Pin_74HC595_NOE, true);
			}
			break;
		}
		default:
			break;
	}
	return reinterpret_cast<void*>(state);
}

static void proccessKey(const uint32_t &key) {
	const Buttons_t button = (Buttons_t)(key & 0x0F);
	const Action_t action = (Action_t)(key & 0xF0);
	if (action == ACTION_RELESE)
		return;
	uint8_t h = 0;
	uint8_t m = 0;
	Clock_rtcGetTime(h, m);
	int16_t time = h*60 + m;
	if (button == BUTTON_UP)
		time++;
	else if (button == BUTTON_DOWN)
		time--;
	if (time < 0)
		time = TIME_MAX - 1;
	else if (time >= TIME_MAX)
		time = 0;
	h = time / 60;
	m = time % 60;
	Clock_rtcSetTime(h, m);
	Clock_setTime(h, m);
}
