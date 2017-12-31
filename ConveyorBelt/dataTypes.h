#ifndef DATA_TYPES_H_
#define DATA_TYPES_H_

enum Cmd {
	REQUEST, READY, WAIT, RELEASE, PLUS, MINUS, STOP, CHAIN, LOCALM, LEFT, RIGHT, DELETE, FINISHED, NOTFOUND
};
enum Source {
	first, SERVER_CLIENT, KEYBOARD, TELNET, CONTROLLER, ALL, last
};
struct parseCommand {
	Cmd commandEnum;
	char cmdString[30];
	Source sourceCb;
};

enum profileState {
	STOPMOTOR, CONSTANTVELOCITY, RAMPDOWN, RAMPUP
};

#define MESSAGE_READY	"Ready\r\n"
#define MESSAGE_WAIT	"Wait\r\n"
#define MESSAGE_RELEASE	"Release\r\n"
#define MESSAGE_REQUEST "Request\r\n"

/* For both TCP and Telnet Servers */
#define REQUEST_MSG_SIZE 50 /* max size of request message */
#define REPLY_MSG_SIZE 50 /* max size of reply message */

/* structure for requests from clients to server */
struct request {
	int reply; /* TRUE = request reply from server */
	int msgLen; /* length of message text */
	char message[REQUEST_MSG_SIZE]; /* message buffer */
};

struct bufferSfd {
	int sfd;
	char ipAddr[20];
	bool wait;
};

#endif
