/*
 * terminal_handler.c
 *
 *  Created on: Feb 22, 2018
 *      Author: mtayler
 */

#include <stdbool.h>
#include <stdio.h>

#include "terminal_handler.h"

_queue_id OpenW(void) {
	TERMINAL_MGMT_PTR msg_ptr = (TERMINAL_MGMT_PTR)_msg_alloc(terminal_mgmt_pool);
	if (! msg_ptr) {
		printf("Couldn't allocate terminal message pointer");
		_task_block();
	}


}
