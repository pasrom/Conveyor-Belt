#ifndef TELNET_DEFINES_H_
#define TELNET_DEFINES_H_

#include <stddef.h>

/* This is for the Telnet Server */
#define TELNET_PORT_NUM 4444 /* server's port number for bind() */
#define TELNET_WORK_PRIORITY 102 /* priority of server's work task */
#define TELNET_STACK_SIZE 10000 /* stack size of server's work task */
#define TELNET_MAX_CONNECTIONS 1 /* max clients connected at a time */

/* This is for the TCP Server */
#define TCP_PORT_NUM 5555 /* server's port number for bind() */
#define TCP_WORK_PRIORITY 101 /* priority of server's work task */
#define TCP_STACK_SIZE 10000 /* stack size of server's work task */
#define TCP_MAX_CONNECTIONS 50 /* max clients connected at a time */

#endif
