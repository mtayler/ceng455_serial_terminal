/* ###################################################################
**     Filename    : os_tasks.c
**     Project     : serial_terminal
**     Processor   : MK64FN1M0VLL12
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2018-02-06, 16:20, # CodeGen: 3
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         TerminalHandler_task - void TerminalHandler_task(os_task_param_t task_init_data);
**
** ###################################################################*/
/*!
** @file os_tasks.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup os_tasks_module os_tasks module documentation
**  @{
*/         
/* MODULE os_tasks */

#include <stdio.h>
#include <string.h>

#include "Cpu.h"
#include "Events.h"
#include "rtos_main_task.h"
#include "os_tasks.h"


#ifdef __cplusplus
extern "C" {
#endif 


/* User includes (#include below this line is not maintained by Processor Expert) */
#include "output.h"
#include "messaging.h"

/*
** ===================================================================
**     Callback    : TerminalHandler_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void TerminalHandler_task(os_task_param_t task_init_data)
{
	printf("Starting TerminalHandler_task\n");
	TERMINAL_MESSAGE_PTR msg_ptr;
	TERMINAL_MGMT_MESSAGE_PTR mgmt_msg_ptr;
	_queue_id terminal_handler_qid;
	_queue_id terminal_mgmt_qid;
	OUTPUT_BUFFER_PTR output = init_output_ptr();
	uint8_t buffer_index = 0;

	QUEUE_STRUCT output_queue;
	_queue_init(&output_queue);

	uint16_t opened_write = 0;

	terminal_mgmt_qid = _msgq_open(TERMINAL_MGMT_QID, 0);
	if (! terminal_mgmt_qid) {
		printf("\nCould not open terminal handler message queue\n");
		_task_block();
	}
	if (! _msgpool_create(sizeof(TERMINAL_MGMT_MESSAGE), 1, 0, 0)) {
		printf("\nCould not create terminal message pool\n");
		_task_block();
	}

	/* Create terminal message queue */
	terminal_handler_qid = _msgq_open(TERMINAL_HANDLER_QID, 0);
	if (! terminal_handler_qid) {
		printf("\nCould not open terminal handler message queue\n");
		_task_block();
	}
	/* Create terminal handler message pool */
	if (! _msgpool_create(sizeof(TERMINAL_MESSAGE), 1, 0, 0)) {
		printf("\nCould not create terminal message pool\n");
		_task_block();
	}

	UART_DRV_SendDataBlocking(terminal_IDX, CODE_clearterm_home, sizeof(CODE_clearterm_home), 1000);

#ifdef PEX_USE_RTOS
	while (1) {
#endif
		/* Handle incoming messages */
		mgmt_msg_ptr = _msgq_poll(terminal_mgmt_qid);

		// If there's a management message
		if (mgmt_msg_ptr) {

			switch (mgmt_msg_ptr->RQST) {
				case OpenW:
					if (! opened_write) {
						opened_write = mgmt_msg_ptr->STREAM;
					} else {
						mgmt_msg_ptr-> RETURN = FALSE;
					}
					break;
				case PutLine:
					if (mgmt_msg_ptr->STREAM == opened_write) {
						bool queued = _queue_enqueue(&output_queue,
								(OUTPUT_LINE)mgmt_msg_ptr->DATA);
						if (queued) {
							mgmt_msg_ptr->RETURN = TRUE;
						} else {
							mgmt_msg_ptr->RETURN = FALSE;
						}
					} else {
						mgmt_msg_ptr->RETURN = TRUE;
					}
					break;
				case Close:
					opened_write = 0;
					mgmt_msg_ptr->RETURN = TRUE;
					break;
				default:
					mgmt_msg_ptr->RETURN = FALSE;
			}
			mgmt_msg_ptr->HEADER.TARGET_QID = mgmt_mgs_ptr->HEADER.SOURCE_QID;
			mgmt_msg_ptr->HEADER.SOURCE_QID = terminal_mgmt_qid;
			_msgq_send(mgmt_msg_ptr);
		}

		OUTPUT_LINE_PTR incoming_line = _queue_dequeue(output_queue);
		if (incoming_line != NULL) {
			UART_DRV_SendDataBlocking(terminal_IDX, incoming_line->LINE, sizeof(incoming_line->LINE), 1500);
		}

		msg_ptr = _msgq_poll(terminal_handler_qid);
		if (! msg_ptr) {
			continue;
		}

		char message = *(msg_ptr->DATA);
		_msg_free(msg_ptr);

		printf("Incoming key: 0x%x\n", message);

		/* Handle input */
		if (message >= 32 && message <= 126) { // printable characters
			output->buffer[buffer_index++] = message;
		} else if (message == '\b') { // Backspace and ^H
			if (buffer_index > 0) { buffer_index -= 1; };
			output->buffer[buffer_index] = '\0';
		} else if (message == '\r') { // new line
			UART_DRV_SendDataBlocking(terminal_IDX, CODE_nextline, sizeof(CODE_nextline), BLOCK_TIMEOUT);
		} else if (message == '\x17') {  // ^W erase word
			while (buffer_index > 0 && output->buffer[--buffer_index] != ' ') {
				output->buffer[buffer_index] = '\0';
			}
			output->buffer[buffer_index] = '\0';
		} // else ignore

		if (message == '\x15' || message == '\r') { // erase line (^U or newline)
			while (buffer_index > 0) {
				output->buffer[--buffer_index] = '\0';
			}
		}

		UART_DRV_SendData(terminal_IDX, (unsigned char*)output->clear, sizeof(OUTPUT_BUFFER_STRUCT));

#ifdef PEX_USE_RTOS   
	}
#endif    
}

/* END os_tasks */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
