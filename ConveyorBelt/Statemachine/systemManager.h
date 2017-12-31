#ifndef SYSTEMMANAGER_H_
#define SYSTEMMANAGER_H_
#include "TcpServer.h"
#include "TcpClient.h"
#include "dataTypes.h"
#include "systemManager.h"
#include "stateMachine.h"
#include "keyboard.h"
#include "myFunctions.h"
#include "hwFunc.h"
#include "parser.h"	
#include "PTPClient.h"
#include "UdpClient.h"
#include "Display.h"
#include "VelocityProfile.h"
#include "Motor.h"
#include "Controller.h"
#include <iostream.h>
#include <algorithm>
#include <iterator>
#include <socket.h>

class TcpServer;
class TcpClient;
class SystemManager;
class Keyboard;
class Display;
class VelocityProfile;
class Controller;
//class PTPClient:

//TelnetServer * myTelnetServer;

//SystemManager * mySystemManger;
int n, m;
SEM_ID semOrder;

void setLocalIp();
class SystemManager {
public:
	SystemManager();
	~SystemManager();

	void requestEvent(Cmd cmd);
	void requestEvent(Cmd cmd, int sFd);
	void connectToServer(char * serverName);
	void initSystem();
	void deinitSystem();
	void setSocketServerToAnswer(int inSFD);
//	void setMaxA(int maxA);
//	void setDirection(bool direction);
	void setTimeStamp();
//	int getMaxA();
//	bool getDirection();
	int getActualSfd();
	int getWaitingSfd();
	TcpServer * myTelnetServer;
	TcpServer * myTCPServer;
	TcpClient * myTCPClient;
	PTPClient* myPTPClient;
	Keyboard * myKeyboard;
	SystemManager * myInSystemManager;
	UdpClient * myInUdpClient;
	Display * myDisplay;
	VelocityProfile * myVelocityChain;
	VelocityProfile * myVelocityHandingOver;
	VelocityProfile * myVelocityLocal;
	Controller * myController;
	//SystemManager * mySystemManager;	
	Parser *myParser;
	bool requestInProcess;
	bool localMode;
	bool chainMode;
	bool orderInProgress;
	int ctrHandingOver;
	struct bufferSfd myBufferSfd[10];
private:
	int actualSfd;
	int waitingSfd;

//	int maxA;
//	bool direction;
	struct timespec timeStamp;
};

#endif // SYSTEMMANAGER_H_