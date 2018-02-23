/*
 * terminal_handler.c
 *
 *  Created on: Feb 22, 2018
 *      Author: mtayler
 */

#include <stdbool.h>
#include <stdio.h>

#include "terminal_handler.h"
#include "messaging.h"

_queue_id terminal_handler_mgmt_local_qid;
_pool_id temrinal_handler_mgmt_local_pool;

void terminal_handler_mgmt_init(void) {
	terminal_handler_mgmt_local_qid = _msgq_open(USER_TASK_QID, 0);
	if (! terminal_handler_mgmt_local_qid) {
		printf("\nCould not open terminal handler mgmt local message queue\n");
		_task_block();
	}
}


_queue_id OpenW(void) {
	TERMINAL_MGMT_MESSAGE_PTR msg_ptr = (TERMINAL_MGMT_MESSAGE_PTR)_msg_alloc(terminal_mgmt_pool);
	if (! msg_ptr) {
		printf("Couldn't allocate terminal mgmt pointer");
		_task_block();
	}

	msg_ptr->HEADER.SOURCE_QID = terminal_handler_mgmt_local_qid;
	msg_ptr->HEADER.TARGET_QID = TERMINAL_MGMT_QID;
	msg_ptr->TASK_ID = _task_get_id();
	msg_ptr->RQST = R_OpenW;
	msg_ptr->HEADER.SIZE = sizeof(msg_ptr->HEADER) + sizeof(msg_ptr->DATA) + sizeof(msg_ptr->RQST) + 1;

	if (! _msgq_send(msg_ptr)) {
		printf("Couldn't send OpenW message");
		_task_block();
	}

	do {
		msg_ptr = (TERMINAL_MGMT_MESSAGE_PTR)_msgq_receive(terminal_handler_mgmt_local_qid, MGMT_WAIT);
	} while (msg_ptr && msg_ptr->RQST != R_OpenW );

	_queue_id qid;

	if (msg_ptr) {
		qid = msg_ptr->HEADER.SOURCE_QID;
	} else {
		qid = 0;
		printf("Not opened for write\n");
	}
	printf("[UserTask]: Received back MGMT message: %s (%d)\n", R_request_to_str(msg_ptr->RQST), qid);
	_msg_free(msg_ptr);
	return qid;
}


bool _putline(_queue_id qid, char line[LINE_LENGTH]) {
	TERMINAL_MGMT_MESSAGE_PTR msg_ptr = (TERMINAL_MGMT_MESSAGE_PTR)_msg_alloc(terminal_mgmt_pool);
	if (! msg_ptr) {
		printf("Couldn't allocate terminal mgmt pointer\n");
		_task_block();
	}

	msg_ptr->HEADER.SOURCE_QID = terminal_handler_mgmt_local_qid;
	msg_ptr->HEADER.TARGET_QID = TERMINAL_MGMT_QID;
	msg_ptr->RQST = R_PutLine;
	msg_ptr->TASK_ID = _task_get_id();
	msg_ptr->DATA = line;
	_msgq_send(msg_ptr);

	do {
		msg_ptr = (TERMINAL_MGMT_MESSAGE_PTR)_msgq_receive(terminal_handler_mgmt_local_qid, MGMT_WAIT);
	} while (msg_ptr && msg_ptr->RQST != R_PutLine );
	bool result = FALSE;
	if (msg_ptr) {
		result = msg_ptr->RETURN;
	}
	printf("[UserTask]: Received back MGMT message: %s (%d)\n", R_request_to_str(msg_ptr->RQST), result);
	_msg_free(msg_ptr);
	return result;
}

bool OpenR(_queue_id stream_no) {
	TERMINAL_MGMT_MESSAGE_PTR msg_ptr = (TERMINAL_MGMT_MESSAGE_PTR)_msg_alloc(terminal_mgmt_pool);
	if (! msg_ptr) {
		printf("Couldn't allocate terminal mgmt pointer\n");
		_task_block();
	}
	msg_ptr->HEADER.SOURCE_QID = terminal_handler_mgmt_local_qid;
	msg_ptr->HEADER.TARGET_QID = TERMINAL_MGMT_QID;
	msg_ptr->RQST = R_OpenR;
	msg_ptr->TASK_ID = _task_get_id();
	_msgq_send(msg_ptr);

	do {
		msg_ptr = (TERMINAL_MGMT_MESSAGE_PTR)_msgq_receive(terminal_handler_mgmt_local_qid, MGMT_WAIT);
	} while (msg_ptr && msg_ptr->RQST != R_OpenR );

	bool result = FALSE;
	if (msg_ptr) {
		result = msg_ptr->RETURN;
	}
	printf("[UserTask]: Received back MGMT message: %s (%d)\n", R_request_to_str(msg_ptr->RQST), result);
	_msg_free(msg_ptr);
	return result;
}

bool _getline(char * line) {
	TERMINAL_MGMT_MESSAGE_PTR msg_ptr = (TERMINAL_MGMT_MESSAGE_PTR)_msg_alloc(terminal_mgmt_pool);
	if (! msg_ptr) {
		printf("Couldn't allocate terminal mgmt pointer\n");
		_task_block();
	}
	msg_ptr->HEADER.SOURCE_QID = terminal_handler_mgmt_local_qid;
	msg_ptr->HEADER.TARGET_QID = TERMINAL_MGMT_QID;
	msg_ptr->RQST = R_GetLine;
	msg_ptr->TASK_ID = _task_get_id();
	_msgq_send(msg_ptr);

	do {     // Get response of request
		msg_ptr = (TERMINAL_MGMT_MESSAGE_PTR)_msgq_receive(terminal_handler_mgmt_local_qid, MGMT_WAIT);
	} while (msg_ptr && msg_ptr->RQST != R_GetLine);

	bool result = FALSE;
	if (msg_ptr && msg_ptr->RETURN) {
		printf("[UserTask/GetLine]: Received good %s (%d)\n", R_request_to_str(msg_ptr->RQST), msg_ptr->RETURN);
		printf("[UserTask/GetLine]: Waiting for line to be returned\n");
		do {      // If good, wait for actual line (no timeout, user input)
			_msg_free(msg_ptr);
			msg_ptr = (TERMINAL_MGMT_MESSAGE_PTR)_msgq_receive(terminal_handler_mgmt_local_qid, 0);
		} while (msg_ptr && msg_ptr->RQST != R_SentLine);
		if (msg_ptr) {
			printf("[UserTask/GetLine]: Received line\n");
			strcpy(line, (char *)msg_ptr->DATA);
			free(msg_ptr->DATA);
			result = TRUE;
		} else {
			result = FALSE;
		}
	} else {      // If response is bad
		result = FALSE;
	}
	printf("[UserTask]: Received back MGMT message: %s (%d)\n", R_request_to_str(msg_ptr->RQST), result);
	_msg_free(msg_ptr);
	return result;
}


bool Close(void) {
	TERMINAL_MGMT_MESSAGE_PTR msg_ptr = (TERMINAL_MGMT_MESSAGE_PTR)_msg_alloc(terminal_mgmt_pool);
	if (! msg_ptr) {
		printf("Couldn't allocate terminal mgmt pointer\n");
		_task_block();
	}

	msg_ptr->HEADER.SOURCE_QID = terminal_handler_mgmt_local_qid;
	msg_ptr->HEADER.TARGET_QID = TERMINAL_MGMT_QID;
	msg_ptr->RQST = R_Close;
	msg_ptr->TASK_ID = _task_get_id();
	_msgq_send(msg_ptr);

	do {
		msg_ptr = (TERMINAL_MGMT_MESSAGE_PTR)_msgq_receive(terminal_handler_mgmt_local_qid, MGMT_WAIT);
	} while (msg_ptr && msg_ptr->RQST != R_Close);
	bool result = FALSE;
	if (msg_ptr) {
		result = msg_ptr->RETURN;
	}
	printf("[UserTask]: Received back MGMT message: %s (%d)\n", R_request_to_str(msg_ptr->RQST), result);
	_msg_free(msg_ptr);
	return result;
}
