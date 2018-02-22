/*
 * messaging.h
 *
 *  Created on: Feb 20, 2018
 *      Author: mtayler
 */

#ifndef SOURCES_MESSAGING_H_
#define SOURCES_MESSAGING_H_

#include <message.h>

#define TERMINAL_MGMT_QID (1)
#define TERMINAL_HANDLER_QID (2)

enum REQUEST {
	R_OpenR = 0,
	R_GetLine,
	R_OpenW,
	R_PutLine,
	R_Close
};

typedef struct terminal_mgmt_message
{
	MESSAGE_HEADER_STRUCT HEADER;
	enum REQUEST          RQST;
	bool                  RETURN;
	void *                DATA;
} TERMINAL_MGMT_MESSAGE, * TERMINAL_MGMT_PTR;

/* This structure contains a data field and a message header structure */
typedef struct terminal_message
{
   MESSAGE_HEADER_STRUCT   HEADER;
   char*                   DATA;
} TERMINAL_MESSAGE, * TERMINAL_MESSAGE_PTR;

_pool_id terminal_message_pool;
_pool_id terminal_mgmt_pool;

#endif /* SOURCES_MESSAGING_H_ */
