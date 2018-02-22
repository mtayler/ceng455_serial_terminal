/*
 * terminal_handler.h
 *
 *  Created on: Feb 20, 2018
 *      Author: mtayler
 */

#ifndef SOURCES_TERMINAL_HANDLER_H_
#define SOURCES_TERMINAL_HANDLER_H_

#include <queue.h>

#include "messaging.h"

typedef struct output_line {
	QUEUE_ELEMENT_STRUCT HEADER;
	char                 LINE[100];
} OUTPUT_LINE, * OUTPUT_LINE_PTR;

_queue_id OpenW(void);

#endif /* SOURCES_TERMINAL_HANDLER_H_ */
