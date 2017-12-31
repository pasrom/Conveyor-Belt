#include "TcpServer.h"
#include "systemManager.h"

TcpServer::TcpServer(SystemManager *systemManager) {
	myInSystemManager = systemManager;
	myParser = new Parser;
	myInTcpServer = this;
	return;
}

TcpServer::~TcpServer() {
	shutdown(sFd, 2);
	close(sFd);
	shutdown(FdWorkerTask[0], 2);
	close(FdWorkerTask[0]);
	taskDelete(taskID);
	delete myParser;
	printf("TcpServer Dekonstruktor\n");
	return;
}

void TcpServer::initTask() {
	int j;
	for (j = 0; j < sizeof(FdWorkerTask) / sizeof(FdWorkerTask[0]); ++j) {
		FdWorkerTask[j] = -2; /* init with error*/
		workerTaskDelete[j] = false;
	}
	taskID = taskSpawn(getTaskName(), 110, VX_FP_TASK, 0x1000, (FUNCPTR) TcpServer::tcpServerWrapper, (int) myInTcpServer, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	printf("taskID Server %i\n", taskID);
}

void TcpServer::parseCommand(int sFd, int inIX) {
	char strTemp[100] = "";
	int ret;
	strcpy(strTemp, myParser->returnIp(textBuffer_a, getType()));
	if (strlen(strTemp) > 0) {
		printf("IP to connect to right: %s \n", strTemp);
		myInSystemManager->connectToServer(strTemp);
		workerTaskDelete[inIX] = true;
	} else {
		Cmd temp = myParser->parseCmd(textBuffer_a, getType());
		myInSystemManager->requestEvent(temp, sFd);
	}
}

void TcpServer::wrapperTask(int sFdWorkTask, /* server's socket fd */
char * address, /* client's socket address */
u_short port, /* client's socket port */
TcpServer* obj, int inIX) {
	TcpServer* myself = (TcpServer*) obj;
	myself->wrapperWorkTask(sFdWorkTask, address, port, inIX);
}

void TcpServer::tcpServerWrapper(TcpServer* obj) {
	TcpServer* myself = (TcpServer*) obj;
	myself->tcpServer();
}

void TcpServer::send(Cmd cmd, int sendSFD) {
	char str[REPLY_MSG_SIZE];
	int found = 0;
	int j;
	sprintf(str, "%s", myParser->parseCmd(cmd));
	/* search for first empty file descriptor */
	for (j = 0; j < sizeof(FdWorkerTask) / sizeof(FdWorkerTask[0]); ++j) {
		if (FdWorkerTask[j] == sendSFD) {
			found = 1;
			break;
		}
	}
	/* only try to send if the sfd is the right! */
	if (found == 1) {
		if (write(sendSFD, str, strlen(str)) == ERROR)
			perror("write");
	} else {
		perror("write");
	}
}

void TcpServer::tcpServer() {
	/* cast the object back to the class */
	struct sockaddr_in serverAddr; /* server's socket address */
	struct sockaddr_in clientAddr; /* client's socket address */
	int sockAddrSize; /* size of socket address structure */
	/* To do: make logic for more than one connection*/
	int i = 0;
	char workName[16]; /* name of work task */
	/* set up the local address */
	sockAddrSize = sizeof(struct sockaddr_in);
	bzero((char *) &serverAddr, sockAddrSize);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_len = (u_char) sockAddrSize;
	serverAddr.sin_port = htons(getPort());
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	printf("Task %s Server Startet\n\r", getTaskName());
	/* create a TCP-based socket */
	if ((sFd = socket(AF_INET, SOCK_STREAM, 0)) == ERROR) {
		perror("socket");
		close(sFd);
	}

	/* bind socket to local address */
	if (bind(sFd, (struct sockaddr *) &serverAddr, sockAddrSize) == ERROR) {
		perror("bind");
		close(sFd);
	}
	/* create queue for client connection requests */

	if (listen(sFd, getMaxConnections()) == ERROR) {
		perror("listen");
		close(sFd);
	}
	ix = 0;

	/* accept new connect requests and spawn tasks to process them */
	FOREVER {
		socklen_t len = sizeof(clientAddr);
		int j;
		bool found = false;
		/* search for first empty file descriptor */
		for (j = 0; j < sizeof(FdWorkerTask) / sizeof(FdWorkerTask[0]); ++j) {
			if (FdWorkerTask[j] < -1) {
				ix = j;
				found = true;
				break;
			}
		}
		/* TO DO:!! do error handling if array is too small!!! */
		if (found) {
			printf("ix 2: %i durchlauf: %i\n", ix, j);
			if ((FdWorkerTask[ix] = accept(sFd, (struct sockaddr *) &clientAddr, &len)) == ERROR) {
				perror("accept");
				close(sFd);
			}
			sprintf(workName, "%sWork%d", getTaskName(), ix);
			printf("FdWorkerTask[i]: %i\n", FdWorkerTask[ix]);
			if (iDWorkerTask[ix] = taskSpawn(workName, getWorkTaskPrio(),
			VX_FP_TASK, getStackSize(), (FUNCPTR) TcpServer::wrapperTask, FdWorkerTask[ix], (int) inet_ntoa(clientAddr.sin_addr),
					ntohs(clientAddr.sin_port), (int) myInTcpServer, ix, 0, 0, 0, 0, 0) == ERROR) {
				/* if taskSpawn fails, close fd and return to top of loop */
				perror("taskSpawn");
				close(FdWorkerTask[ix]);
			}
		} else {
			beLazy(500);
		}
	}
}

void TcpServer::wrapperWorkTask(int sFdWorkTask, /* server's socket fd */
char * address, /* client's socket address */
u_short port, /* client's socket port */
int inIX) {
	/* request/message from client */
	int nRead; /* number of bytes read */
	struct sockaddr_in addr;
	socklen_t addr_size = sizeof(struct sockaddr_in);
	/* read client request, display message */
	while ((nRead = fioRdString(sFdWorkTask, (char *) &textBuffer_a, sizeof(textBuffer_a))) > 0) {

		int res = getpeername(sFdWorkTask, (struct sockaddr *) &addr, &addr_size);
		char *clientip = new char[20];
		strcpy(clientip, inet_ntoa(addr.sin_addr));
		printf("Client Message: IP %s, Port %i: %s length: %i Sfd: \n\r", clientip, port, textBuffer_a, nRead, sFdWorkTask);
		if (nRead == ERROR) {/* error from read() */
			perror("read");
		}
		parseCommand(sFdWorkTask, inIX);
		if (workerTaskDelete[inIX])
			break;
	}
	printf("2 wrapperWorkTask%i \n", inIX);
	if (sFdWorkTask == FdWorkerTask[inIX]) {
		FdWorkerTask[inIX] = -2;
		printf("Ok for deleting the task: %i\n", FdWorkerTask[inIX]);
	}
	shutdown(sFdWorkTask, 2);
	close(sFdWorkTask); /* close server socket connection */
	workerTaskDelete[inIX] = false;
	taskDelete(0);
}

void TcpServer::setReplyMsgSize(int in) {
	replyMsgSize = in;
}
void TcpServer::setRequestMsgSize(int in) {
	requestMsgSize = in;
}
void TcpServer::setMaxConnections(int in) {
	maxConnections = in;
}
void TcpServer::setPort(unsigned short in) {
	port = in;
}
void TcpServer::setStackSize(int in) {
	stackSize = in;
}
void TcpServer::setWorkTaskPrio(int in) {
	workTaskPrio = in;
}
void TcpServer::setType(Source in) {
	type = in;
}
void TcpServer::setTaskName(char * in) {
	strcpy(taskName, in);
}
int TcpServer::getReplyMsgSize() {
	return replyMsgSize;
}
int TcpServer::getRequestMsgSize() {
	return requestMsgSize;
}
int TcpServer::getMaxConnections() {
	return maxConnections;
}
unsigned short TcpServer::getPort() {
	return port;
}
int TcpServer::getStackSize() {
	return stackSize;
}
int TcpServer::getWorkTaskPrio() {
	return workTaskPrio;
}
Source TcpServer::getType() {
	return type;
}
char * TcpServer::getTaskName() {
	return taskName;
}
