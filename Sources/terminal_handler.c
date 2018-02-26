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
bool terminal_handler_mgmt_initialized = FALSE;

void terminal_handler_mgmt_init(void) {
	if (! terminal_handler_mgmt_initialized) {
		terminal_handler_mgmt_local_qid = _msgq_open(TERMINAL_HANDLER_MGMT_QID, 0);
		if (! terminal_handler_mgmt_local_qid) {
			printf("\nCould not open terminal handler mgmt message queue\n");
			_task_block();
		}
		terminal_handler_mgmt_initialized = TRUE;
	}
}


_queue_id OpenW(void) {
	TERMINAL_MGMT_MESSAGE_PTR msg_ptr = (TERMINAL_MGMT_MESSAGE_PTR)_msg_alloc(terminal_mgmt_pool);
	if (! msg_ptr) {
		printf("Couldn't allocate OpenW message");
		_task_block();
	}

	msg_ptr->HEADER.SOURCE_QID = terminal_handler_mgmt_local_qid;
	msg_ptr->HEADER.TARGET_QID = TERMINAL_MGMT_QID;
	msg_ptr->HEADER.SIZE = sizeof(TERMINAL_MGMT_MESSAGE);
	msg_ptr->TASK_ID = _task_get_id();
	msg_ptr->RQST = R_OpenW;
	msg_ptr->HEADER.SIZE = sizeof(msg_ptr->HEADER) + sizeof(msg_ptr->DATA) + sizeof(msg_ptr->RQST) + 1;

	if (! _msgq_send(msg_ptr)) {
		printf("Couldn't send OpenW message");
		_task_block();
	}

	do {
		msg_ptr = (TERMINAL_MGMT_MESSAGE_PTR)_msgq_receive(terminal_handler_mgmt_local_qid, MGMT_WAIT);
//		if (msg_ptr && msg_ptr->RQST != R_OpenW) {	// if we received the wrong message, resend
//			_msgq_send(msg_ptr);
//		}
	} while (msg_ptr && msg_ptr->RQST != R_OpenW);

	_queue_id qid;

	if (msg_ptr) {
		qid = msg_ptr->HEADER.SOURCE_QID;
	} else {
		qid = 0;
		printf("Not opened for write\n");
	}
	_mutex_lock(print_mutex);
	printf("[UserTask/OpenW]: Received back MGMT message: %s (%d)\n", R_request_to_str(msg_ptr->RQST), qid);
	_mutex_unlock(print_mutex);
	_msg_free(msg_ptr);
	return qid;
}


bool _putline(_queue_id qid, char line[LINE_LENGTH]) {
	TERMINAL_MGMT_MESSAGE_PTR msg_ptr = (TERMINAL_MGMT_MESSAGE_PTR)_msg_alloc(terminal_mgmt_pool);
	if (! msg_ptr) {
		printf("Couldn't allocate _putline message\n");
		_task_block();
	}

	msg_ptr->HEADER.SOURCE_QID = terminal_handler_mgmt_local_qid;
	msg_ptr->HEADER.TARGET_QID = TERMINAL_MGMT_QID;
	msg_ptr->HEADER.SIZE = sizeof(TERMINAL_MGMT_MESSAGE);
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
		_mutex_lock(print_mutex);
		printf("[UserTask/_putline]: Received back MGMT message: %s (%d)\n", R_request_to_str(msg_ptr->RQST), result);
		_mutex_unlock(print_mutex);
		_msg_free(msg_ptr);
	} else {
		result = FALSE;
	}
	return result;
}

bool OpenR(_queue_id stream_no) {
	TERMINAL_MGMT_MESSAGE_PTR msg_ptr = (TERMINAL_MGMT_MESSAGE_PTR)_msg_alloc(terminal_mgmt_pool);
	if (! msg_ptr) {
		printf("Couldn't allocate OpenR message\n");
		_task_block();
	}
	msg_ptr->HEADER.SOURCE_QID = stream_no;
	msg_ptr->HEADER.TARGET_QID = TERMINAL_MGMT_QID;
	msg_ptr->HEADER.SIZE = sizeof(TERMINAL_MGMT_MESSAGE);
	msg_ptr->RQST = R_OpenR;
	msg_ptr->TASK_ID = _task_get_id();
	_msgq_send(msg_ptr);

	do {
		msg_ptr = (TERMINAL_MGMT_MESSAGE_PTR)_msgq_receive(stream_no, MGMT_WAIT);
//		if (msg_ptr && msg_ptr->RQST != R_OpenR) {	// if we received the wrong message, resend
//			_msgq_send(msg_ptr);
//		}
	} while (msg_ptr && msg_ptr->RQST != R_OpenR );

	bool result = FALSE;
	if (msg_ptr) {
		result = msg_ptr->RETURN;
	}
	_mutex_lock(print_mutex);
	printf("[UserTask/OpenR]: Received back MGMT message: %s (%d)\n", R_request_to_str(msg_ptr->RQST), result);
	_mutex_unlock(print_mutex);
	_msg_free(msg_ptr);
	return result;
}

bool _getline(char * line, _queue_id qid) {
	TERMINAL_MGMT_MESSAGE_PTR msg_ptr = (TERMINAL_MGMT_MESSAGE_PTR)_msg_alloc(terminal_mgmt_pool);
	if (! msg_ptr) {
		printf("Couldn't allocate _getline message\n");
		_task_block();
	}
	msg_ptr->HEADER.SOURCE_QID = terminal_handler_mgmt_local_qid;
	msg_ptr->HEADER.TARGET_QID = TERMINAL_MGMT_QID;
	msg_ptr->HEADER.SIZE = sizeof(TERMINAL_MGMT_MESSAGE);
	msg_ptr->RQST = R_GetLine;
	msg_ptr->TASK_ID = _task_get_id();
	_msgq_send(msg_ptr);

	do {     // Get response of request
		msg_ptr = (TERMINAL_MGMT_MESSAGE_PTR)_msgq_receive(terminal_handler_mgmt_local_qid, MGMT_WAIT);
	} while (msg_ptr && msg_ptr->RQST != R_GetLine);

	bool result = FALSE;
	if (msg_ptr && msg_ptr->RETURN) {  // If true response, get returned line
		_mutex_lock(print_mutex);
		printf("[UserTask/_getline]: Received line \"%s\"\n", (char*)msg_ptr->DATA);
		_mutex_unlock(print_mutex);
		strcpy(line, (char *)msg_ptr->DATA);
		free(msg_ptr->DATA);
		result = TRUE;
	} else {               // If false response
		result = FALSE;
	}
	if (msg_ptr) {
		_mutex_lock(print_mutex);
		printf("[UserTask/_getline]: Received back MGMT message: %s (%d)\n", R_request_to_str(msg_ptr->RQST), result);
		_mutex_unlock(print_mutex);
		_msg_free(msg_ptr);
	}
	return result;
}


bool Close(void) {
	TERMINAL_MGMT_MESSAGE_PTR msg_ptr = (TERMINAL_MGMT_MESSAGE_PTR)_msg_alloc(terminal_mgmt_pool);
	if (! msg_ptr) {
		printf("Couldn't allocate Close message\n");
		_task_block();
	}

	msg_ptr->HEADER.SOURCE_QID = terminal_handler_mgmt_local_qid;
	msg_ptr->HEADER.TARGET_QID = TERMINAL_MGMT_QID;
	msg_ptr->HEADER.SIZE = sizeof(TERMINAL_MGMT_MESSAGE);
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
	_mutex_lock(print_mutex);
	printf("[UserTask/Close]: Received back MGMT message: %s (%d)\n", R_request_to_str(msg_ptr->RQST), result);
	_mutex_unlock(print_mutex);
	_msg_free(msg_ptr);
	return result;
}
