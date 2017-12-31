#ifndef TCP_SERVER_H
#define TCP_SERVER_H

/*
 * TcpServer.h
 *
 *  Created on: ${date}
 *      Author: ${user}
 */

/*
 * Description:
 */

#include <taskLib.h>

#include "TcpServerDefines.h"
#include "systemManager.h"
#include "ICommunication.h"

class SystemManager;

class TcpServer: public ICommunication {
public:
	/* Default constructor */
	TcpServer(SystemManager *systemManager);
	/* Default destructor */
	~TcpServer();
	void send(Cmd cmd, int sendSFD);
	void initTask();
	static void tcpServerWrapper(TcpServer* obj);
	void tcpServer();
	static void wrapperTask(int sFdWorkTask, char * address, u_short port, TcpServer* obj, int inIX);
	/* setter methods for the server */
	void wrapperWorkTask(int sFdWorkTask, char * address, u_short port, int inIX);
	void setReplyMsgSize(int in);
	void setRequestMsgSize(int in);
	void setMaxConnections(int in);
	void setPort(unsigned short in);
	void setStackSize(int in);
	void setWorkTaskPrio(int in);
	void setType(Source in);
	void setTaskName(char * in);

	/* getter methods for the server */
	int getReplyMsgSize();
	int getRequestMsgSize();
	int getMaxConnections();
	unsigned short getPort();
	int getStackSize();
	int getWorkTaskPrio();
	Source getType();
	char * getTaskName();
protected:

private:
	/* instance of objects */
	SystemManager * myInSystemManager;
	Parser *myParser;
	/* Create an instance of the own class */
	TcpServer *myInTcpServer;
	/* methodes */
	void parseCommand(int sFd, int inIX);
	/* variable declaration */
	char taskName[20];
	char textBuffer_a[REQUEST_MSG_SIZE];
	/* TO DO: make logic for more than one connection!! */
	int FdWorkerTask[11];/* socket descriptor from accept */
	int iDWorkerTask[11];
	bool workerTaskDelete[11];
	int replyMsgSize, requestMsgSize, maxConnections, stackSize, workTaskPrio;
	unsigned short port;
	Source type;
	int sFd;
	int ix; /* counter for work task names */
	int taskID;
};
#endif
