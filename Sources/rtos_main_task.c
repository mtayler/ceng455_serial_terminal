/* ###################################################################
 **     Filename    : rtos_main_task.c
 **     Project     : serial_terminal
 **     Processor   : MK64FN1M0VLL12
 **     Component   : Events
 **     Version     : Driver 01.00
 **     Compiler    : GNU C Compiler
 **     Date/Time   : 2018-02-06, 16:18, # CodeGen: 1
 **     Abstract    :
 **         This is user's event module.
 **         Put your event handler code here.
 **     Settings    :
 **     Contents    :
 **         main_task - void main_task(os_task_param_t task_init_data);
 **
 ** ###################################################################*/
/*!
 ** @file rtos_main_task.c
 ** @version 01.00
 ** @brief
 **         This is user's event module.
 **         Put your event handler code here.
 */
/*!
 **  @addtogroup rtos_main_task_module rtos_main_task module documentation
 **  @{
 */
/* MODULE rtos_main_task */

#include "Cpu.h"
#include "Events.h"
#include "rtos_main_task.h"
#include "os_tasks.h"

#ifdef __cplusplus
extern "C" {
#endif 


/* User includes (#include below this line is not maintained by Processor Expert) */

#include <stdio.h>

#include <terminal_manager.h>

/* Initialization of Processor Expert components function prototype */
#ifdef MainTask_PEX_RTOS_COMPONENTS_INIT
extern void PEX_components_init(void);
#endif 

/*
 ** ===================================================================
 **     Callback    : main_task
 **     Description : Task function entry.
 **     Parameters  :
 **       task_init_data - OS task parameter
 **     Returns : Nothing
 ** ===================================================================
 */
void main_task(os_task_param_t task_init_data)
{

	/* Initialization of Processor Expert components (when some RTOS is active). DON'T REMOVE THIS CODE!!! */
#ifdef MainTask_PEX_RTOS_COMPONENTS_INIT
	PEX_components_init();
#endif 
	/* End of Processor Expert components initialization.  */

	char in_line[LINE_LENGTH];
	char out_line[LINE_LENGTH+20];

	_mutex_init(print_mutex, 0);

	terminal_manager_init();

	_queue_id local_qid = _msgq_open(10, 0);
	if (! local_qid) {
		_mutex_lock(print_mutex);
		printf("\nCould not open main task message queue\n");
		_mutex_unlock(print_mutex);
		_task_block();
	}

	_time_delay(100);

	printf("Unauthorized _getline returns: %d\n", _getline(0));
	OpenR(local_qid);
	printf("Unauthorized _putline returns: %d\n", _putline(TERMINAL_MGMT_QID, ""));
	_queue_id qid = OpenW();

	// Create clients to listen to individual character inputs
	_task_id read_task_id = _task_create(0, READTASK_TASK, 0);
	if (read_task_id == 0) {
		_mutex_lock(print_mutex);
		printf("[MainTask]: Could not create a read task\n");
		_mutex_unlock(print_mutex);
		_task_block();
	}

	_task_id read_close_task_id = _task_create(0, READCLOSETASK_TASK, 0);
	if (read_close_task_id == 0) {
			_mutex_lock(print_mutex);
			printf("[MainTask]: Could not create a read task\n");
			_mutex_unlock(print_mutex);
			_task_block();
	}
	_time_delay(1000);

#ifdef PEX_USE_RTOS
	while (1) {
#endif
		_mutex_lock(print_mutex);
		printf("[MainTask]: Trying getline\n");
		_mutex_unlock(print_mutex);
		// Get a line from the terminal, then put line with some wrapper text
		if (_getline(in_line)) {
			sprintf(out_line, "Received: \"%s\"", in_line);
			_putline(qid, out_line);
		} else {
			_mutex_lock(print_mutex);
			printf("[MainTask]: Couldn't get line\n");
			_mutex_unlock(print_mutex);
		}
		OSA_TimeDelay(100);
	}
}

/* END rtos_main_task */

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
