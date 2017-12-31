#ifndef I_COMMUNICATION_H
#define I_COMMUNICATION_H
/* include all things we need for our server and clients */
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <assert.h>
#include "dataTypes.h"
#include <vxWorks.h>
#include <semLib.h>
#include <sockLib.h>
#include <inetLib.h>
#include <ioLib.h>
#include <fiolib.h>
#include <string.h>
#include <taskLib.h>
#include <hostLib.h>
#include "dataTypes.h"
#include <time.h>
#include "parser.h"

class ICommunication {
private:

public:
//	 ICommunication();
//	 ~ICommunication();
	//virtual void initTask() = 0;	
//	virtual void send(enum Cmd) = 0;	
	virtual void setReplyMsgSize(int in) = 0;
	virtual void setRequestMsgSize(int in)= 0;
	virtual void setMaxConnections(int in)= 0;
	virtual void setPort(unsigned short in)= 0;
	virtual void setStackSize(int in)= 0;
	virtual void setType(Source in)= 0;
	virtual void setTaskName(char * in)= 0;
};
#endif
