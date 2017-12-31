#include "TcpClient.h"
#include "systemManager.h"

TcpClient::TcpClient(SystemManager *systemManager) {
	myInSystemManager = systemManager;
	myParser = new Parser;
	myInTcpClient = this;
	ix = 0;
	deleteTask = false;
	return;
}

TcpClient::~TcpClient() {
	/* delete resources */
	delete myParser;
	//delete myInTcpClient;
	//delete myInSystemManager;
	return;
}

void TcpClient::stopClient() {
	deleteTask = true;
	shutdown(sFd, 2);
	close(sFd);
	taskDelete(taskID);
	taskDelete(FdWorkerTask[0]);
}

void TcpClient::parseCommand() {
	Cmd temp = myParser->parseCmd(textBuffer_a, getType());
	myInSystemManager->requestEvent(temp);
}

void TcpClient::wrapperTask(int sFdWorkTask, /* server's socket fd */
char * address, /* Server's socket address */
u_short port, /* Server's socket port */
TcpClient* obj) {
	TcpClient* myself = (TcpClient*) obj;
	myself->wrapperWorkTask(sFdWorkTask, address, port);
	printf("jou\n\r");
}

void TcpClient::send(Cmd cmd) {
	char str[REPLY_MSG_SIZE];
	int ret;
	sprintf(str, "%s", myParser->parseCmd(cmd));
	printf("%s, %d\n", str, FdWorkerTask[0]);
	ret = write(FdWorkerTask[0], str, strlen(str));
	printf("FdWorkerTask[0] Client: %i %s ret %i", FdWorkerTask[0], str, ret);
	if (ret == ERROR)
		perror("write");
}

STATUS TcpClient::tcpClientStart() {
	struct request myRequest; /* request to send to server */
	struct sockaddr_in serverAddr; /* server's socket address */
	char replyBuf[REPLY_MSG_SIZE]; /* buffer for reply */
	char reply; /* if TRUE, expect reply back */
	int sockAddrSize; /* size of socket address structure */
	int retAddr, retName;
	int i = 0;

	char workName[16]; /* name of work task */
	/* create client's socket */
	if ((sFd = socket(AF_INET, SOCK_STREAM, 0)) == ERROR) {
		perror("socket");
		return ERROR;
	}
	/* bind not required - port number is dynamic */
	/* build server socket address */
	sockAddrSize = sizeof(struct sockaddr_in);
	bzero((char *) &serverAddr, sockAddrSize);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_len = (u_char) sockAddrSize;
	serverAddr.sin_port = htons(getPort());

	retAddr = inet_addr(getServerName());
	if (retAddr == ERROR) {
		perror("unknown server name: inet_addr\n");
	} else {
		serverAddr.sin_addr.s_addr = (u_int32_t) retAddr;
	}
	retName = hostGetByName(getServerName());
	if (retName == ERROR) {
		perror("unknown server name: hostGetByName\n");

	} else {
		serverAddr.sin_addr.s_addr = (u_int32_t) retName;
	}
	if (retName == ERROR && retAddr == ERROR) {
		close(sFd);
		return ERROR;
	}
	/* connect to server */
	if (connect(sFd, (struct sockaddr *) &serverAddr, sockAddrSize) == ERROR) {
		perror("connect");
		close(sFd);

	}
	FdWorkerTask[0] = sFd;
	printf("in client: %i\n", sFd);
	sprintf(workName, "%sWork%d", getTaskName(), ix++);
	if (taskSpawn(workName, getWorkTaskPrio(), VX_FP_TASK, getStackSize(), (FUNCPTR) TcpClient::wrapperTask, FdWorkerTask[0],
			(int) inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port), (int) myInTcpClient, 0, 0, 0, 0, 0, 0) == ERROR) {
		/* if taskSpawn fails, close fd and return to top of loop */
		perror("taskSpawn");
		close(FdWorkerTask[0]);
	}
	return OK;
}

void TcpClient::wrapperWorkTask(int sFdWorkTask, /* server's socket fd */
char * address, /* Server's socket address */
u_short port /* Server's socket port */
) {

	/* request/message from server */
	int nRead; /* number of bytes read */
	printf("IP to Connect to %s\n", address);
	struct sockaddr_in addr;
	socklen_t addr_size = sizeof(struct sockaddr_in);
	/* read client request, display message */
	while ((nRead = fioRdString(sFdWorkTask, (char *) &textBuffer_a, sizeof(textBuffer_a))) > 0) {
		int res = getpeername(sFdWorkTask, (struct sockaddr *) &addr, &addr_size);
		char *clientip = new char[20];
		strcpy(clientip, inet_ntoa(addr.sin_addr));
		printf("SERVER MESSAGE: IP %s, Port %i:", clientip, port);
		printf("%s, %i\n", textBuffer_a, sFdWorkTask);

		if (nRead == ERROR) {/* error from read() */
			perror("read");
		}
		/* testing */
		parseCommand();
		if (deleteTask)
			break;
	}
	close(sFdWorkTask); /* close server socket connection */
	taskDelete(0);
}

void TcpClient::setReplyMsgSize(int in) {
	replyMsgSize = in;
}
void TcpClient::setRequestMsgSize(int in) {
	requestMsgSize = in;
}
void TcpClient::setMaxConnections(int in) {
	maxConnections = in;
}
void TcpClient::setPort(unsigned short in) {
	port = in;
}
void TcpClient::setStackSize(int in) {
	stackSize = in;
}
void TcpClient::setWorkTaskPrio(int in) {
	workTaskPrio = in;
}
void TcpClient::setType(Source in) {
	type = in;
}
void TcpClient::setServerName(char * in) {
	strcpy(servername, in);
}
void TcpClient::setTaskName(char * in) {
	strcpy(taskName, in);
}
int TcpClient::getReplyMsgSize() {
	return replyMsgSize;
}
int TcpClient::getRequestMsgSize() {
	return requestMsgSize;
}
int TcpClient::getMaxConnections() {
	return maxConnections;
}
unsigned short TcpClient::getPort() {
	return port;
}
int TcpClient::getStackSize() {
	return stackSize;
}
int TcpClient::getWorkTaskPrio() {
	return workTaskPrio;
}
Source TcpClient::getType() {
	return type;
}
char * TcpClient::getServerName() {
	return servername;
}
char * TcpClient::getTaskName() {
	return taskName;
}
