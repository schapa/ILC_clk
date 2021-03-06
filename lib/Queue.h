/*
 * Queue.h
 *
 *  Created on: Oct 13, 2016
 *      Author: shapa
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "Events.h"

/**
 * @brief Push event to Queue
 * @param[in] type     event type
 * @param[in] data     data for event. This must be static or allocated and must NOT be a pointer to stack data
 * @param[in] dispose  dispose method. Could be Null
 *
 */
void EventQueue_Push(EventTypes_e type, void *data, onEvtDispose_f dispose);

/**
 * @brief Pend event on Queue
 * @param[out] pointer to event
 */
void EventQueue_Pend(Event_t *event);

/**
 * @brief Dispose event
 */
void EventQueue_Dispose(Event_t *event);

#ifdef __cplusplus
}
#endif
