#ifndef PTP_UDP_DEF_H
#define PTP_UDP_DEF_H

/* definesUDP.h - header used by both UDP server and client examples */

/* structure used both for sending and receiving, includes address and port */

#define PTP_CURRENT_VERSION '1'

#define SYNC_MESSAGE 'S'
#define FOLLOW_UP_MESSAGE 'F'
#define DELAY_REQ_MESSAGE 'D'
#define DELAY_RESP_MESSAGE 'R'

#define PORT_NUM 5432

#define MESSAGE_SIZE 20

struct requestPTP {
	char PTP_version; /* PTP version used */
	char message_type; /* PTP mesage type is here */
	int epoch_number; /* part of timestamp */
	time_t seconds; /* part of timestamp */
	int nanoseconds; /* part of timestamp */
	char inetAddr[INET_ADDR_LEN]; /* address of the other communication participant */
};
#endif
