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
#include "output.h"

#define LINE_LENGTH (100)

typedef struct received_line {
	MESSAGE_HEADER_STRUCT HEADER;
	char*                 LINE;
} RECEIVED_LINE, * RECEIVED_LINE_PTR;

typedef struct output_line {
	QUEUE_ELEMENT_STRUCT HEADER;
	char                 LINE[LINE_LENGTH];
} OUTPUT_LINE, * OUTPUT_LINE_PTR;


void terminal_handler_mgmt_init();
bool OpenR(_queue_id);
bool _getline(char* string);
_queue_id OpenW(void);
bool _putline(_queue_id qid, char line[LINE_LENGTH]);
bool Close(void);

#endif /* SOURCES_TERMINAL_HANDLER_H_ */
