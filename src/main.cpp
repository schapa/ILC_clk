
#include <stdio.h>
#include <stdlib.h>

#include "bsp.h"
#include "Queue.h"
#include "timers.h"
#include "systemTimer.h"
#include "clock.h"


static void onTimerPush(uint32_t id);

typedef void* (*state_f)(const Event_t&);

static void* state_On(const Event_t &event);
static void* state_Off(const Event_t &event);

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
				Clock_setTime(m, System_getUptime());
			}
			break;
		case EVENT_ADC:
			if (intVal < 36000) {
				state = state_Off;
				BSP_SetPinVal(BSP_Pin_POWER_SWITCH, true);
				BSP_SetPinVal(BSP_Pin_74HC595_NOE, false);
			}
			break;
		default:
			break;
	}
	return reinterpret_cast<void*>(state);
}

static void* state_Off(const Event_t &event) {
	state_f state = state_Off;
	switch (event.type) {
		case EVENT_KEY:
			state = state_On;
			BSP_SetPinVal(BSP_Pin_POWER_SWITCH, false);
			BSP_SetPinVal(BSP_Pin_74HC595_NOE, true);
			break;
		default:
			break;
	}
	return reinterpret_cast<void*>(state);
}
