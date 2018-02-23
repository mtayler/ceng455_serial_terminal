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
#include "terminal_handler.h"

// Vector for opened tasks
typedef struct read_entry {
	_task_id  TASK_ID;
	_queue_id QID;
} READ_ENTRY, * READ_ENTRY_PTR;
#define VECTOR_TYPE READ_ENTRY_PTR
#define VECTOR_NAME stream
#include "vector.h"

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
	OUTPUT_BUFFER_PTR output_buffer = init_output_ptr();
	uint8_t buffer_index = 0;

	QUEUE_STRUCT output_queue;
	_queue_init(&output_queue, 0);

	vec_stream_t read_queue;        // list of streams opened for read
	_task_id opened_write = 0;     // task id opened for write

	vec_stream_init(read_queue);

	terminal_mgmt_qid = _msgq_open(TERMINAL_MGMT_QID, 0);
	if (! terminal_mgmt_qid) {
		printf("\nCould not open terminal handler mgmt message queue\n");
		_task_block();
	}
	terminal_mgmt_pool = _msgpool_create(sizeof(TERMINAL_MGMT_MESSAGE), 1, 0, 0);
	if (! terminal_mgmt_pool) {
		printf("\nCould not create terminal mgmt pool\n");
		_task_block();
	}

	/* Create terminal message queue */
	terminal_handler_qid = _msgq_open(TERMINAL_HANDLER_QID, 0);
	if (! terminal_handler_qid) {
		printf("\nCould not open terminal handler message queue\n");
		_task_block();
	}
	/* Create terminal handler message pool */
	terminal_handler_pool = _msgpool_create(sizeof(TERMINAL_MESSAGE), 1, 0, 0);
	if (! terminal_handler_pool) {
		printf("\nCould not create terminal message pool\n");
		_task_block();
	}


#ifdef PEX_USE_RTOS
	while (1) {
#endif
		// Handle incoming management message
		mgmt_msg_ptr = _msgq_poll(terminal_mgmt_qid);
		msg_ptr = _msgq_poll(terminal_handler_qid);
		if (mgmt_msg_ptr) {
			printf("[TerminalHandler]: Received MGMT message: %s\n", R_request_to_str(mgmt_msg_ptr->RQST));
			switch (mgmt_msg_ptr->RQST) {
				case R_OpenW: {
					if (! opened_write) {                           // Set the write task and return our input queue id
						opened_write = mgmt_msg_ptr->TASK_ID;
						mgmt_msg_ptr->RETURN = TRUE;
						mgmt_msg_ptr->DATA = &terminal_mgmt_qid;
					} else {                                        // Already opened for write, do nothing
						mgmt_msg_ptr->RETURN = FALSE;
						mgmt_msg_ptr->DATA = 0;
					}
					break;
				}
				case R_PutLine: {
					if (mgmt_msg_ptr->TASK_ID == opened_write) {     // If the message source has opened for write
						OUTPUT_LINE_PTR outline = malloc(sizeof(OUTPUT_LINE));
						strncpy(outline->LINE, (char*)mgmt_msg_ptr->DATA, sizeof(outline->LINE));
						bool queued = _queue_enqueue(&output_queue, (QUEUE_ELEMENT_STRUCT_PTR)outline);
						if (queued) {
							mgmt_msg_ptr->RETURN = TRUE;
						} else {
							mgmt_msg_ptr->RETURN = FALSE;
						}
					} else {
						mgmt_msg_ptr->RETURN = TRUE;
					}
					break;
				}
				case R_OpenR: {
					// Add stream to vector of opened
					READ_ENTRY_PTR read_entry = malloc(sizeof(READ_ENTRY));
					read_entry->TASK_ID = mgmt_msg_ptr->TASK_ID,
					read_entry->QID = mgmt_msg_ptr->HEADER.SOURCE_QID;
					bool contained = FALSE;
					for (size_t i=0; i < vec_stream_size(read_queue); i++) {
						if (vec_stream_get(read_queue, i)->TASK_ID == mgmt_msg_ptr->TASK_ID) {
							mgmt_msg_ptr->RETURN = FALSE;
							contained = TRUE;
							break;
						}
					}
					if (! contained) {
						vec_stream_append(read_queue, read_entry);
						mgmt_msg_ptr->RETURN = TRUE;
					}
					break;
				}
				case R_GetLine: {
					// Check if sending Queue is registered for read
					// If registered don't send anything and keep call blocking
					// until line entered. Otherwise return RETURN = FALSE
					bool authorized = FALSE;
					for (size_t i=0; i < vec_stream_size(read_queue); i++) {
						if (mgmt_msg_ptr->HEADER.SOURCE_QID == vec_stream_get(read_queue, i)->QID) {
							authorized = TRUE;
							break;
						}
					}
					if (! authorized) {
						mgmt_msg_ptr->RETURN = FALSE;
					} else {
						mgmt_msg_ptr->RETURN = TRUE;
					}
					break;
				}
				case R_Close: {
					if (mgmt_msg_ptr->TASK_ID == opened_write) {
						opened_write = 0;
					}
					// Remove stream from read vector if exists
					for (size_t i=0; i < vec_stream_size(read_queue); i++) {
						if (vec_stream_get(read_queue, i)->TASK_ID == mgmt_msg_ptr->TASK_ID) {
							free(vec_stream_get(read_queue, i));
							vec_stream_del(read_queue, i);
							break;
						}
					}
					mgmt_msg_ptr->RETURN = TRUE;
					break;
				}
				default:
					mgmt_msg_ptr->RETURN = FALSE;
			}
			mgmt_msg_ptr->HEADER.TARGET_QID = mgmt_msg_ptr->HEADER.SOURCE_QID;
			mgmt_msg_ptr->HEADER.SOURCE_QID = terminal_mgmt_qid;
			mgmt_msg_ptr->HEADER.SIZE = sizeof(*mgmt_msg_ptr);
			_msgq_send(mgmt_msg_ptr);
		}

		if (msg_ptr && vec_stream_size(read_queue) > 0) {
			char message = *(msg_ptr->DATA);
			_msg_free(msg_ptr);
			// Forward message to all reading tasks
			for (size_t i=0; i < vec_stream_size(read_queue); i++) {
				RECEIVED_LINE_MESSAGE_PTR rl = (RECEIVED_LINE_MESSAGE_PTR)_msg_alloc(terminal_handler_pool);
				rl->HEADER.TARGET_QID = vec_stream_get(read_queue, i)->QID;
				rl->HEADER.SOURCE_QID = terminal_handler_qid;
				rl->HEADER.SIZE = sizeof(RECEIVED_LINE_MESSAGE);
				rl->CHARACTER = message;
				_msgq_send(rl);
			}

			/* Handle input */
			if (message >= 32 && message <= 126) { // printable characters
				output_buffer->buffer[buffer_index++] = message;
			} else if (message == '\b' || message == '\x33') { // Backspace and ^H (and Delete)
				if (buffer_index > 0) { buffer_index -= 1; };
				output_buffer->buffer[buffer_index] = '\0';
			} else if (message == '\r') { // new line
				UART_DRV_SendDataBlocking(terminal_IDX, CODE_nextline, sizeof(CODE_nextline), BLOCK_TIMEOUT);
				// Iterate over list of queues opened for read and send line
				for (size_t i=0; i < vec_stream_size(read_queue); i++) {
					TERMINAL_MGMT_MESSAGE_PTR line_msg_ptr = (TERMINAL_MGMT_MESSAGE_PTR)_msg_alloc(terminal_mgmt_pool);
					line_msg_ptr->HEADER.TARGET_QID = vec_stream_get(read_queue, i)->QID;
					line_msg_ptr->HEADER.SOURCE_QID = terminal_mgmt_qid;
					line_msg_ptr->HEADER.SIZE = sizeof(TERMINAL_MGMT_MESSAGE);
					line_msg_ptr->RQST = R_SentLine;
					char* line = malloc(sizeof(char[LINE_LENGTH])); // make new string to send address in message
					strcpy(line, output_buffer->buffer);
					line_msg_ptr->DATA = line;
					_msgq_send(line_msg_ptr);
				}
			} else if (message == '\x17') {  // ^W erase word
				while (buffer_index > 0 && output_buffer->buffer[--buffer_index] != ' ') {
					output_buffer->buffer[buffer_index] = '\0';
				}
				output_buffer->buffer[buffer_index] = '\0';
			} // else ignore

			if (message == '\x15' || message == '\r') { // erase line (^U or newline)
				while (buffer_index > 0) {
					output_buffer->buffer[--buffer_index] = '\0';
				}
			}
			UART_DRV_SendData(terminal_IDX, (unsigned char*)output_buffer->clear, sizeof(OUTPUT_BUFFER_STRUCT));
		}

		OUTPUT_LINE_PTR queued_line = (OUTPUT_LINE_PTR)_queue_dequeue(&output_queue);
		if (queued_line) {
			UART_DRV_SendDataBlocking(terminal_IDX, (unsigned char*)queued_line->LINE, sizeof(queued_line->LINE), 2000);
			free(queued_line);
			UART_DRV_SendDataBlocking(terminal_IDX, CODE_nextline, sizeof(CODE_nextline), 1000);
		}

		if (! msg_ptr && ! mgmt_msg_ptr) {
			// Give time for other tasks to run if there's no incoming messages
			_time_delay(10);
		}
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
