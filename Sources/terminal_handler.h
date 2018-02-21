/*
 * terminal_handler.h
 *
 *  Created on: Feb 20, 2018
 *      Author: mtayler
 */

#ifndef SOURCES_TERMINAL_HANDLER_H_
#define SOURCES_TERMINAL_HANDLER_H_

#include "messaging.h"

_queue_id OpenW(void) {
	TERMINAL_MESSAGE_PTR msg_ptr = (TERMINAL_MESSAGE_PTR)_msg_alloc(terminal_message_pool);
	if (msg_ptr == NULL) {
		printf("Couldn't allocate terminal message pointer");
		_task_block();
	}

	msg_ptr->HEADER.TARGET_QID = _msgq_get_id(0, TERMINAL_HANDLER_QID);
	msg_ptr->HEADER.SIZE = sizeof(MESSAGE_HEADER_STRUCT) + sizeof(input_buffer) + 1;
	msg_ptr->DATA = input_buffer;

	bool result = _msgq_send(msg_ptr);

	if (result != TRUE) {
		printf("\nCould not send a message\n");
		_task_block();
	}
}

#endif /* SOURCES_TERMINAL_HANDLER_H_ */
