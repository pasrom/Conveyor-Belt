#ifndef TCP_ClIENT_DEFINES_H_
#define TCP_ClIENT_DEFINES_H_

#include <stddef.h>

/* This is for the TCP Client */
#define TCP_ClIENT_PORT_NUM 5555 /* server's port number for bind() */
#define TCP_ClIENT_WORK_PRIORITY 107 /* priority of server's work task */
#define TCP_ClIENT_STACK_SIZE 10000 /* stack size of server's work task */
#define TCP_ClIENT_MAX_CONNECTIONS 1 /* max clients connected at a time */

/* For TCP Clients */
#define REQUEST_MSG_SIZE 50 /* max size of request message */
#define REPLY_MSG_SIZE 50 /* max size of reply message */

/* structure for requests from clients to server */
//struct request {
//    int reply; /* TRUE = request reply from server */
//    int msgLen; /* length of message text */
//    char message[REQUEST_MSG_SIZE]; /* message buffer */
//};
#endif
