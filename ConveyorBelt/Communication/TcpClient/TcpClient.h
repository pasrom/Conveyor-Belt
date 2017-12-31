#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include "TcpClientDefines.h"
#include "systemManager.h"
#include "ICommunication.h"

class SystemManager;

class TcpClient: public ICommunication {
public:
	/* Default constructor */
	TcpClient(SystemManager *systemManager);
	/* Default destructor */
	~TcpClient();

	void send(Cmd cmd);
	STATUS tcpClientStart();
	void stopClient();

	/* setter methods for the server */
	void wrapperWorkTask(int sFdWorkTask, char * address, u_short port);
	static void wrapperTask(int sFdWorkTask, char * address, u_short port, TcpClient* obj);
	void setReplyMsgSize(int in);
	void setRequestMsgSize(int in);
	void setMaxConnections(int in);
	void setPort(unsigned short in);
	void setStackSize(int in);
	void setWorkTaskPrio(int in);
	void setType(Source in);
	void setServerName(char * in);
	void setTaskName(char * in);

	/* getter methods for the client */
	int getReplyMsgSize();
	int getRequestMsgSize();
	int getMaxConnections();
	unsigned short getPort();
	int getStackSize();
	int getWorkTaskPrio();
	Source getType();
	char * getServerName();
	char * getTaskName();

protected:

private:
	/* instance of objects */
	SystemManager * myInSystemManager;
	Parser *myParser;
	/* Create an instance of the own class */
	TcpClient *myInTcpClient;
	/* methodes */
	void parseCommand();

	/* variable declaration */
	char textBuffer_a[REQUEST_MSG_SIZE];
	/* TO DO: make logic for more than one connection!! */
	int FdWorkerTask[1];/* socket descriptor from accept */
	int ix;
	char taskName[20];
	char servername[20];
	int replyMsgSize, requestMsgSize, maxConnections, stackSize, workTaskPrio;
	unsigned short port;
	Source type;
	int sFd;
	int taskID;
	bool deleteTask;
};
#endif
