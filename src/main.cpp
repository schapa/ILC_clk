
#include <stdio.h>
#include <stdlib.h>

#include "bsp.h"
#include "Queue.h"
#include "timers.h"


static void onTimerPush(uint32_t id);

int main(int argc, char* argv[]) {

	Timer_init(onTimerPush);
	BSP_Init();
	while (true) {
		Event_t event;
		EventQueue_Pend(&event);
		BSP_FeedWatchdog();
		uint32_t intVal = (uint32_t)event.data;
		switch (event.type) {
			case EVENT_TIMCALL:
				Timer_onTimerCb(intVal);
				break;
			case EVENT_ADC: {
				break;
			}
			default:
				break;
		}
		EventQueue_Dispose(&event);
	}
	return 0;
}

static void onTimerPush(uint32_t id) {
	EventQueue_Push(EVENT_TIMCALL, (void*)id, NULL);
}
