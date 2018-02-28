/*
 * terminal_handler.h
 *
 *  Created on: Feb 20, 2018
 *      Author: mtayler
 */

#ifndef SOURCES_TERMINAL_MANAGER_H_
#define SOURCES_TERMINAL_MANAGER_H_

#include <queue.h>
#include <mutex.h>

#include "messaging.h"
#include "output.h"

// Vector for opened tasks
typedef struct read_entry {
	_task_id  TASK_ID;
	_queue_id QID;
} READ_ENTRY, * READ_ENTRY_PTR;

#define VECTOR_TYPE READ_ENTRY_PTR
#define VECTOR_NAME stream
#include "vector.h"

#define LINE_LENGTH (100)

MUTEX_STRUCT_PTR print_mutex;

typedef struct received_char {
	MESSAGE_HEADER_STRUCT HEADER;
	char                  CHARACTER;
} RECEIVED_CHAR_MESSAGE, * RECEIVED_CHAR_MESSAGE_PTR;

typedef struct send_line {
	QUEUE_ELEMENT_STRUCT HEADER;
	_queue_id            TARGET_QID;
} SEND_LINE_DEST, * SEND_LINE_DEST_PTR;

typedef struct output_line {
	QUEUE_ELEMENT_STRUCT HEADER;
	char                 LINE[LINE_LENGTH];
} OUTPUT_LINE, * OUTPUT_LINE_PTR;


void terminal_manager_init(void);
bool OpenR(_queue_id qid);
bool _getline(char* string);
_queue_id OpenW(void);
bool _putline(_queue_id qid, char line[LINE_LENGTH]);
bool Close(void);

#endif /* SOURCES_TERMINAL_MANAGER_H_ */
