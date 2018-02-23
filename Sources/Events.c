/* ###################################################################
**     Filename    : Events.c
**     Project     : serial_terminal
**     Processor   : MK64FN1M0VLL12
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2018-02-06, 16:13, # CodeGen: 0
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file Events.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         
/* MODULE Events */

#include "Cpu.h"
#include "Events.h"
#include "rtos_main_task.h"
#include "os_tasks.h"

#include "stdio.h"

#ifdef __cplusplus
extern "C" {
#endif 


/* User includes (#include below this line is not maintained by Processor Expert) */
#include "messaging.h"

/*
** ===================================================================
**     Callback    : terminal_RxCallback
**     Description : This callback occurs when data are received.
**     Parameters  :
**       instance - The UART instance number.
**       uartState - A pointer to the UART driver state structure
**       memory.
**     Returns : Nothing
** ===================================================================
*/
void terminal_RxCallback(uint32_t instance, void * uartState)
{
	TERMINAL_MESSAGE_PTR msg_ptr = (TERMINAL_MESSAGE_PTR)_msg_alloc(terminal_handler_pool);
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

/* END Events */

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
