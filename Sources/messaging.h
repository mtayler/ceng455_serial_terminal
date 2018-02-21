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
	OpenR = 0,
	GetLine,
	OpenW,
	PutLine,
	Close
};

typedef struct terminal_mgmt_message
{
	MESSAGE_HEADER_STRUCT HEADER;
	REQUEST               REQUEST;
	uint32_t              STREAM;
	bool                  RETURN;
	void *                DATA;
};

/* This structure contains a data field and a message header structure */
typedef struct terminal_message
{
   MESSAGE_HEADER_STRUCT   HEADER;
   char*                   DATA;
} TERMINAL_MESSAGE, * TERMINAL_MESSAGE_PTR;

_pool_id terminal_message_pool;

#endif /* SOURCES_MESSAGING_H_ */
