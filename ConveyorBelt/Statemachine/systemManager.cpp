#include "systemManager.h"

SystemManager * mySystemManager;
StateMachine * myStateMachine;
SystemManager::SystemManager() {
	// Initialize table for all diagrams, event time in ms (POSIX)
	// The maximum size of the table is defined in stateTable.h:
	// MAXDIA = 9, MAXLINES = 66
	// Should these be exceeded, please correct!
	//myInSystemManager = this;
	int countLocalM = 0, LocalM = 0;
	int countChainM = 0, ChainM = 1;
	int countSwitchM = 0, SwitchM = 2;
	int countBeltInMotionM = 0, BetlInMotionM = 3;
	int countPerformingM = 0, PerformingM = 4;
	for (int k = 0; k < sizeof(myBufferSfd) / sizeof(myBufferSfd[0]); ++k) {
		myBufferSfd[k].sfd = -2;
		myBufferSfd[k].wait = false;
		strcpy(myBufferSfd[k].ipAddr, "");
	}

	ctrHandingOver = 0;
	localMode = false;
	chainMode = false;
	orderInProgress = false;
	requestInProcess = false;
	semOrder = semMCreate(SEM_Q_PRIORITY);
	//-------------------Local-MODE-----------------------
	tab[LocalM][countLocalM] = new TableEntry("SelectLocalMode", "Idle", "LocalModeS", 0, myAction00, myConditionTrue);
	tab[LocalM][++countLocalM] = new TableEntry("Idle", "Idle", "SetSpeedLocalP", 0, myAction01, myConditionLocalM);
	tab[LocalM][++countLocalM] = new TableEntry("Idle", "Idle", "SetSpeedLocalM", 0, myAction02, myConditionLocalM);
	tab[LocalM][++countLocalM] = new TableEntry("Idle", "BeltInMotion", "BeltInMotionL", 0, myAction03, myConditionLocalM);
	tab[LocalM][++countLocalM] = new TableEntry("Idle", "BeltInMotion", "BeltInMotionR", 0, myAction04, myConditionLocalM);
	tab[LocalM][++countLocalM] = new TableEntry("BeltInMotion", "Idle", "StopBelt", 0, myAction05, myConditionLocalM);
	tab[LocalM][++countLocalM] = new TableEntry("Idle", "SelectLocalMode", "ChainMode", 0, myAction06, myConditionLocalM);
	//-------------------Chain-MODE-----------------------
	tab[ChainM][countChainM] = new TableEntry("SelectChainMode", "Idle", "ChainModeS", 0, myAction10, myConditionTrue);
	tab[ChainM][++countChainM] = new TableEntry("Idle", "SelectChainMode", "LocalMode", 0, myAction11, myConditionChainM);
	tab[ChainM][++countChainM] = new TableEntry("Idle", "HandingOverL", "request", 0, myAction12, myConditionChainM);
	tab[ChainM][++countChainM] = new TableEntry("HandingOverL", "BeltInMotion", "constf", 0, myAction13, myConditionChainM);
	tab[ChainM][++countChainM] = new TableEntry("BeltInMotion", "Waiting", "StopBelt", 0, myAction14, myConditionChainM);
	tab[ChainM][++countChainM] = new TableEntry("Waiting", "Waiting", "wait", 0, myAction15, myConditionChainM);
	tab[ChainM][++countChainM] = new TableEntry("Waiting", "Idle", "CancelOrder", 0, myAction19, myConditionChainM);
	tab[ChainM][++countChainM] = new TableEntry("Waiting", "HandingOverR", "ready", 0, myAction16, myConditionChainM);
	tab[ChainM][++countChainM] = new TableEntry("HandingOverR", "HandingOverR", "constf", 0, myAction17, myConditionChainM);
	tab[ChainM][++countChainM] = new TableEntry("HandingOverR", "Idle", "release", 0, myAction18, myConditionChainM);
	//-------------------Switch-MODE-----------------------
	tab[SwitchM][countSwitchM] = new TableEntry("startSystem", "Init", "InitSystem", 0, myAction20, myConditionTrue);
	tab[SwitchM][++countSwitchM] = new TableEntry("Init", "Init", "Error", 0, myAction21, myConditionTrue);
	tab[SwitchM][++countSwitchM] = new TableEntry("Init", "ChainMode", "ChainMode", 0, myAction22, myConditionTrue);
	tab[SwitchM][++countSwitchM] = new TableEntry("Init", "LocalMode", "LocalMode", 0, myAction23, myConditionTrue);
	tab[SwitchM][++countSwitchM] = new TableEntry("LocalMode", "ChainMode", "ChainModeL", 0, myAction24, myConditionTrue);
	tab[SwitchM][++countSwitchM] = new TableEntry("ChainMode", "LocalMode", "LocalModeC", 0, myAction25, myConditionTrue);
	//-------------------Belt-In-Motion-----------------------
	tab[BetlInMotionM][countBeltInMotionM] = new TableEntry("startBelt", "RampUp", "BeltInMotionL", 0, myAction30, myConditionTrue);
	tab[BetlInMotionM][++countBeltInMotionM] = new TableEntry("startBelt", "RampUp", "BeltInMotionR", 0, myAction30, myConditionTrue);
	tab[BetlInMotionM][++countBeltInMotionM] = new TableEntry("RampUp", "startBelt", "StopBelt", 0, myAction31, myConditionTrue);
	tab[BetlInMotionM][++countBeltInMotionM] = new TableEntry("RampUp", "ConstantVelocity", "rampupf", 0, myAction32, myConditionTrue);
	tab[BetlInMotionM][++countBeltInMotionM] = new TableEntry("ConstantVelocity", "startBelt", "StopBelt", 0, myAction33, myConditionTrue);
	tab[BetlInMotionM][++countBeltInMotionM] = new TableEntry("ConstantVelocity", "RampDown", "constf", 0, myAction34, myConditionTrue);
	tab[BetlInMotionM][++countBeltInMotionM] = new TableEntry("RampDown", "startBelt", "StopBelt", 0, myAction35, myConditionTrue);
	tab[BetlInMotionM][++countBeltInMotionM] = new TableEntry("RampDown", "startBelt", "rampdownf", 0, myAction36, myConditionTrue);
	//-------------------Performing-----------------------
	tab[PerformingM][countPerformingM] = new TableEntry("startPerforming", "Performing", "request", 0, myAction40, myConditionChainM);
	tab[PerformingM][++countPerformingM] = new TableEntry("Performing", "Performing", "request", 0, myAction41, myConditionChainM);
	tab[PerformingM][++countPerformingM] = new TableEntry("Performing", "startPerforming", "release", 0, myAction42, myConditionChainM);

	// Initialize timer names for all diagrams
	// Timer names are always Timer followed by the diagram number
	timerNames[ChainM] = "Timer1";
	timerNames[BetlInMotionM] = "Timer3";

	// Initialize line numbers for all diagrams
	lines[LocalM] = countLocalM + 1;
	lines[ChainM] = countChainM + 1;
	lines[SwitchM] = countSwitchM + 1;
	lines[BetlInMotionM] = countBeltInMotionM + 1;
	lines[PerformingM] = countPerformingM + 1;

	// Initialize first state for all diagrams
	actualState[LocalM] = "SelectLocalMode";
	actualState[ChainM] = "SelectChainMode";
	actualState[SwitchM] = "startSystem";
	actualState[BetlInMotionM] = "startBelt";
	actualState[PerformingM] = "startPerforming";

	// Set the actual number of diagrams
	diagrams = PerformingM + 1;

	// Create instance of state machine
	myStateMachine = new StateMachine;

	// Start timer for each diagram which needs one in the first state!
	myStateMachine->diaTimerTable[ChainM]->startTimer(tab[ChainM][0]->eventTime);
	myStateMachine->diaTimerTable[BetlInMotionM]->startTimer(tab[BetlInMotionM][0]->eventTime);
	return;
}

SystemManager::~SystemManager() {
	return;
}

void SystemManager::requestEvent(Cmd command) {
	printf("requestEvent im SystemManager %i\n\r", command);
	switch (command) {
	case CHAIN:
		myStateMachine->sendEvent("ChainMode");
		break;
	case LOCALM:
		myStateMachine->sendEvent("LocalMode");
		break;
	case PLUS:
		myStateMachine->sendEvent("SetSpeedLocalP");
		break;
	case MINUS:
		myStateMachine->sendEvent("SetSpeedLocalM");
		break;
	case LEFT:
		if (localMode)
			myStateMachine->sendEvent("BeltInMotionL");
		break;
	case RIGHT:
		if (localMode)
			myStateMachine->sendEvent("BeltInMotionR");
		break;
	case STOP:
		if (localMode) {
			myStateMachine->sendEvent("StopBelt");
		} else {
			myStateMachine->sendEvent("CancelOrder");
			printf(" Stopping in Chain-Mode not allowed!");
		}
		break;
	case REQUEST:
		myStateMachine->sendEvent("request");
		break;
	case READY:
		myStateMachine->sendEvent("ready");
		break;
	case RELEASE:
		myStateMachine->sendEvent("release");
		break;
	case WAIT:
		myStateMachine->sendEvent("wait");
		break;
	case DELETE:
//		deinitSystem();
		break;
	case FINISHED:
		myStateMachine->sendEvent("StopBelt");
		break;
	case NOTFOUND:
		myStateMachine->sendEvent("Error");
		break;
	default:
		myStateMachine->sendEvent("Error");
		break;
	}
}

void SystemManager::requestEvent(Cmd command, int sFd) {
	/*  */
	int j = 0;
	int k = 0;
	int ctr = 0;
	bool breakIt = false;
	int strCmp = 0;
	if (command == REQUEST && !localMode) {
		printf("sFd im sysM: %i requestInProcess %i\n", sFd, requestInProcess);
		struct sockaddr_in addr;
		socklen_t addr_size = sizeof(struct sockaddr_in);
		int res = getpeername(sFd, (struct sockaddr *) &addr, &addr_size);
		char *clientip = new char[20];
		strcpy(clientip, inet_ntoa(addr.sin_addr));
		printf("clientip %s\n", clientip);
		semTake(semOrder, WAIT_FOREVER);
		for (j = 0; j < sizeof(myBufferSfd) / sizeof(myBufferSfd[0]); ++j) {
			/* selber auftrag mit zaehlen */
			if (myBufferSfd[j].sfd == sFd) {
				ctr++;
			}
			/* auftrag bereits da, wenn in der uebergabe ist ? */
			/* auftrag bereits da, vom selben Client ? -> Abbruch!!! */
			strCmp = strcmp(myBufferSfd[j].ipAddr, clientip);
			if (strCmp == 0) {
				mySystemManager->myTCPServer->send(NOTFOUND, sFd);
				breakIt = true;
				break;
			}
			/* auftrag bereits zwei mal da -> nicht annehmen! */
			if (ctr > 0) {
				mySystemManager->myTCPServer->send(NOTFOUND, sFd);
				breakIt = true;
				break;
			}
			if (myBufferSfd[j].sfd == -2) {
				if (j == 0) {
					orderInProgress = true;
				}
				myBufferSfd[j].sfd = sFd;
				myBufferSfd[j].wait = j;
				strcpy(myBufferSfd[j].ipAddr, inet_ntoa(addr.sin_addr));
				break;
			} else {
				k++;
			}
		}
		if (j > k) {
			perror("Auftragspuffer voll!\n");
			breakIt = true;
		}
		semGive(semOrder);
	}
	if (!breakIt)
		requestEvent(command);
}

int SystemManager::getActualSfd() {
	return myBufferSfd[0].sfd;
}
int SystemManager::getWaitingSfd() {
	int sfd = -2;
	int j;
	for (j = 0; j < sizeof(myBufferSfd) / sizeof(myBufferSfd[0]); ++j) {
		if (myBufferSfd[j].wait) {
			sfd = myBufferSfd[j].sfd;
			myBufferSfd[j].wait = false; /* wait send ! */
			break;
		}
	}
	return sfd;
}

void SystemManager::deinitSystem() {
	printf("deinitSystem called \n");
	myDisplay->deinitTask();
	myKeyboard->deinitTask();
	myPTPClient->stopPTPClient();
	myTCPClient->stopClient();
	delete (myKeyboard);
	beLazy(500);
	printf("deinitSystem called 6 \n");
	delete (myDisplay);
	beLazy(500);
	printf("deinitSystem called 7 \n");
#ifdef __COM
	delete myTelnetServer;
	delete myTCPServer;
#endif
	delete myTCPClient;
	printf("deinitSystem called 8 \n");
	delete (myPTPClient);

	delete (myStateMachine);
	delete (mySystemManager);
	printf("deinitSystem called last! \n");
}

void SystemManager::initSystem() {
	// Set local IP address according to MAC table
	setLocalIp();
	initHardware(0);
	// Create instance of my Keyboard
	myKeyboard = new Keyboard(mySystemManager);
	myKeyboard->initTask();

	myDisplay = new Display(mySystemManager, myStateMachine);
	myDisplay->initTask();

	myVelocityChain = new VelocityProfile(925, 925, 6000, 1800, false);
	myVelocityHandingOver = new VelocityProfile(1, 1, 1000, 100, false);
	myVelocityLocal = new VelocityProfile(925, 925, 6000, 1600, false);
//	myVelocityLocal = new VelocityProfile(1, 1, 4000, 1000, false);

	myTelnetServer = new TcpServer(mySystemManager);
	myTelnetServer->setReplyMsgSize(REPLY_MSG_SIZE);
	myTelnetServer->setRequestMsgSize(REQUEST_MSG_SIZE);
	myTelnetServer->setMaxConnections(TELNET_MAX_CONNECTIONS);
	myTelnetServer->setPort(TELNET_PORT_NUM);
	myTelnetServer->setStackSize(TELNET_STACK_SIZE);
	myTelnetServer->setWorkTaskPrio(TELNET_WORK_PRIORITY);
	myTelnetServer->setType(TELNET);
	myTelnetServer->setTaskName("tTcpTelnet");
	myTelnetServer->initTask();
#define __COM
#ifdef __COM	
	myTCPServer = new TcpServer(mySystemManager);
	myTCPServer->setReplyMsgSize(REPLY_MSG_SIZE);
	myTCPServer->setRequestMsgSize(REQUEST_MSG_SIZE);
	myTCPServer->setMaxConnections(TCP_MAX_CONNECTIONS);
	myTCPServer->setPort(TCP_PORT_NUM);
	myTCPServer->setStackSize(TCP_STACK_SIZE);
	myTCPServer->setWorkTaskPrio(TCP_WORK_PRIORITY);
	myTCPServer->setType(SERVER_CLIENT);
	myTCPServer->setTaskName("tTcpServer");
	myTCPServer->initTask();
#endif	

	myPTPClient = new PTPClient;
	myPTPClient->setTaskName("tPtpClient");
	myPTPClient->startPTPClient();

	myTCPClient = new TcpClient(mySystemManager);
	myTCPClient->setReplyMsgSize(REPLY_MSG_SIZE);
	myTCPClient->setRequestMsgSize(REQUEST_MSG_SIZE);
	myTCPClient->setMaxConnections(TCP_ClIENT_MAX_CONNECTIONS);
	myTCPClient->setPort(TCP_ClIENT_PORT_NUM);
	myTCPClient->setStackSize(TCP_ClIENT_STACK_SIZE);
	myTCPClient->setWorkTaskPrio(TCP_ClIENT_WORK_PRIORITY);
	myTCPClient->setTaskName("tTcpClient");
	myTCPClient->setType(SERVER_CLIENT);

	myController = new Controller(mySystemManager, myStateMachine);
	myController->controllerRun();

}

void SystemManager::connectToServer(char * serverName) {
	myTCPClient->setServerName(serverName);
	myTCPClient->tcpClientStart();
	myDisplay->setInitialization(serverName);
}

//void SystemManager:: setMaxA(int maxA){
//	this->maxA = maxA;
//}
//
//void SystemManager::setDirection(bool direction){
//	this->direction = direction;
//}
//
//int SystemManager::getMaxA(){
//	return this->maxA;
//}
//
//bool SystemManager::getDirection(){
//	return this->direction;
//}

void myAction00() {
//	mySystemManager->localMode = true;
//	mySystemManager->chainMode = false;
	printf(" SelectLocalMode -> Transition00 -> Idle\n\r");
	mySystemManager->myDisplay->setOperationMode("Local-Mode");
	mySystemManager->myDisplay->setStatus("Idle");
	mySystemManager->myDisplay->setMaxSpeed(mySystemManager->myVelocityLocal->getA());
	return;
}
void myAction01() {
	printf(" Idle -> Transition01 -> Idle\n\r");
	mySystemManager->myVelocityLocal->setA(mySystemManager->myVelocityLocal->getA() + 100);
	if (mySystemManager->myVelocityLocal->getA() > 2200) {
		mySystemManager->myVelocityLocal->setA(2200);
	}
	mySystemManager->myDisplay->setMaxSpeed(mySystemManager->myVelocityLocal->getA());
	return;
}
void myAction02() {
	printf(" Idle -> Transition02 -> Idle\n\r");
	mySystemManager->myVelocityLocal->setA(mySystemManager->myVelocityLocal->getA() - 100);
	if (mySystemManager->myVelocityLocal->getA() < 1000) {
		mySystemManager->myVelocityLocal->setA(1000);
	}
	mySystemManager->myDisplay->setMaxSpeed(mySystemManager->myVelocityLocal->getA());
	return;
}
void myAction03() {
	printf(" Idle -> Transition03 -> BeltInMotion\n\r");
	mySystemManager->myVelocityLocal->setDirection(true);
	mySystemManager->myController->setProfile(mySystemManager->myVelocityLocal);
	mySystemManager->myDisplay->setDirection(mySystemManager->myVelocityLocal->getDirection());
	mySystemManager->myDisplay->setStatus("Belt in Motion");
	return;
}
void myAction04() {
	printf(" Idle -> Transition04 -> BeltInMotion\n\r");
	mySystemManager->myVelocityLocal->setDirection(false);
	mySystemManager->myController->setProfile(mySystemManager->myVelocityLocal);
	mySystemManager->myDisplay->setDirection(mySystemManager->myVelocityLocal->getDirection());
	mySystemManager->myDisplay->setStatus("Belt in Motion");
	return;
}
void myAction05() {
	printf(" BeltInMotion -> Transition05 -> Idle\n\r");
	mySystemManager->myDisplay->setStatus("Idle");
	mySystemManager->myController->setSpeedToZero();
	return;
}
void myAction06() {
	myStateMachine->sendEvent("ChainModeL");
	printf(" Idle -> Transition06 -> SelectLocalMode\n\r");
	return;
}

void myAction10() {
	printf(" SelectChainMode -> Transition10 -> Idle\n\r");
	mySystemManager->myDisplay->setOperationMode("Chain-Mode");
	mySystemManager->myDisplay->setMaxSpeed(mySystemManager->myVelocityChain->getA());
	mySystemManager->myDisplay->setDirection(mySystemManager->myVelocityChain->getDirection());
	mySystemManager->myDisplay->setStatus("Idle");
	return;
}

void myAction11() {
	printf(" Idle -> Transition11 -> SelectChainMode\n\r");
	myStateMachine->sendEvent("LocalModeC");
	return;
}
void myAction12() {
	printf(" Idle -> Transition12 -> HandingOverL\n\r");
	mySystemManager->requestInProcess = true;
	mySystemManager->orderInProgress = true;
	printf("myAcion12() %i\n", mySystemManager->getActualSfd());
	mySystemManager->myTCPServer->send(READY, mySystemManager->getActualSfd());
	mySystemManager->myDisplay->setStatus("Handing over LeftN");
	mySystemManager->myController->setProfile(mySystemManager->myVelocityHandingOver);
	mySystemManager->myController->setConstantVelocity();
	return;
}
void myAction13() {
	printf(" HandingOverL -> Transition13 -> BeltInMotion\n\r");
	struct timespec timeStamp;
	mySystemManager->myController->setProfile(mySystemManager->myVelocityChain);
	myStateMachine->sendEvent("BeltInMotionR");
	printf("myAcion13() %i\n", mySystemManager->getActualSfd());
	mySystemManager->orderInProgress = false;
	mySystemManager->myTCPServer->send(RELEASE, mySystemManager->getActualSfd());
	mySystemManager->myDisplay->setStatus("Belt in Motion");
	clock_gettime(CLOCK_REALTIME, &timeStamp);
	mySystemManager->myDisplay->setRecTimestamp(&timeStamp);
	mySystemManager->myController->setSpeedToZero();
	return;
}
void myAction14() {
	printf(" BeltInMotion -> Transition14 -> Waiting\n\r");
	mySystemManager->myTCPClient->send(REQUEST);
	mySystemManager->myController->setSpeedToZero();
	mySystemManager->myDisplay->setStatus("Waiting");
	return;
}
void myAction15() {
	printf(" Waiting -> Transition15 -> Waiting\n\r");
	return;
}
void myAction16() {
	printf(" Waiting -> Transition16 -> HandingOverR\n\r");
	mySystemManager->myDisplay->setStatus("Handing over RightN");
	mySystemManager->myController->setProfile(mySystemManager->myVelocityHandingOver);
	mySystemManager->myController->setConstantVelocity();
	return;
}
void myAction17() {
	printf(" HandingOverR -> Transition17 -> HandingOverR\n\r");
	char temp[30];

	sprintf(temp, "Handing over RightN: %i", mySystemManager->ctrHandingOver);
	mySystemManager->myDisplay->setStatus(temp);
	mySystemManager->myController->setProfile(mySystemManager->myVelocityHandingOver);
	mySystemManager->myController->setConstantVelocity();
	mySystemManager->ctrHandingOver++;
	return;
}
void myAction18() {
	printf(" HandingOverR -> Transition18 -> Idle\n\r");
	struct timespec timeStamp;
	mySystemManager->myDisplay->setStatus("Idle");
	clock_gettime(CLOCK_REALTIME, &timeStamp);
	mySystemManager->myDisplay->setSendTimestamp(&timeStamp);
	mySystemManager->myController->setSpeedToZero();
	mySystemManager->requestInProcess = false;
	mySystemManager->ctrHandingOver = 0;
//	int j;
//	 for ( j = 0; j < sizeof(mySystemManager->myBufferSfd)/sizeof(mySystemManager->myBufferSfd[0]); ++j){
//	        swap(mySystemManager->myBufferSfd[j], mySystemManager->myBufferSfd[j+1]);
//	 }
//	 mySystemManager->myBufferSfd[j-1].sfd = -2;
//	 mySystemManager->myBufferSfd[j-1].wait = false;
	return;
}

void myAction19() {
	printf(" Waiting -> Transition19 -> Idle\n\r");
//	int j;
//	 for ( j = 0; j < sizeof(mySystemManager->myBufferSfd)/sizeof(mySystemManager->myBufferSfd[0]); ++j){
//	        swap(mySystemManager->myBufferSfd[j], mySystemManager->myBufferSfd[j+1]);
//	 }
//	 mySystemManager->myBufferSfd[j-1].sfd = -2;
//	 mySystemManager->myBufferSfd[j-1].wait = false;

	myStateMachine->sendEvent("release");
	mySystemManager->requestInProcess = false;
	mySystemManager->myDisplay->setStatus("Idle");

}

void myAction20() {
	printf(" StartSystem -> Transition20 -> Init\n\r");
	mySystemManager->initSystem();
	mySystemManager->myDisplay->setOperationMode("SelectMode");
	return;
}
void myAction21() {
	printf(" Init -> Transition21 -> Error\n\r");
	return;
}
void myAction22() {
	printf(" Init -> Transition22 -> ChainMode\n\r");
	mySystemManager->localMode = false;
	mySystemManager->chainMode = true;
	myStateMachine->sendEvent("ChainModeS");
	return;
}
void myAction23() {
	printf(" Init -> Transition23 -> LocalMode\n\r");
	mySystemManager->localMode = true;
	mySystemManager->chainMode = false;
	myStateMachine->sendEvent("LocalModeS");
	return;
}
void myAction24() {
	printf(" LocalMode -> Transition24 -> ChainMode\n\r");
	mySystemManager->localMode = false;
	mySystemManager->chainMode = true;
	myStateMachine->sendEvent("ChainModeS");
	return;
}
void myAction25() {
	printf(" ChainMode -> Transition25 -> LocalMode\n\r");
	mySystemManager->localMode = true;
	mySystemManager->chainMode = false;
	myStateMachine->sendEvent("LocalModeS");
	return;
}

void myAction30() {
	printf(" StartBelt -> Transition30 -> RampUp\n\r");
	mySystemManager->myController->Subsystem_initialize();
	mySystemManager->myController->setRampUp();
	return;
}
void myAction31() {
	printf(" RampUp -> Transition31 -> startBelt\n\r");
	mySystemManager->myController->setSpeedToZero();
	return;
}
void myAction32() {
	printf(" RampUp -> Transition32 -> ConstantVelocity\n\r");
	mySystemManager->myController->setConstantVelocity();
	return;
}
void myAction33() {
	printf(" ConstantVelocity -> Transition33 -> startBelt\n\r");
	mySystemManager->myController->setSpeedToZero();
	return;
}
void myAction34() {
	printf(" ConstantVelocity -> Transition34 -> RampDown\n\r");
	mySystemManager->myController->setRampDown();
	return;
}
void myAction35() {
	printf(" RampDown -> Transition35 -> startBelt\n\r");
	mySystemManager->myController->setSpeedToZero();
	return;
}
void myAction36() {
	printf(" RampDown -> Transition36 -> startBelt\n\r");
	Cmd command = FINISHED;
	mySystemManager->myController->setSpeedToZero();
	mySystemManager->requestEvent(command);
	return;
}

void myAction40() {
	printf(" startPerforming -> Transition40 -> Performing\n\r");
	return;
}
void myAction41() {
	printf(" Performing -> Transition41 -> Performing\n\r");
	int j;
	mySystemManager->myTCPServer->send(WAIT, mySystemManager->getWaitingSfd());
	return;
}
void myAction42() {
	printf(" Performing -> Transition42 -> startPerforming\n\r");
	int j;
	semTake(semOrder, WAIT_FOREVER);
	for (j = 0; j < sizeof(mySystemManager->myBufferSfd) / sizeof(mySystemManager->myBufferSfd[0]); ++j) {
		swap(mySystemManager->myBufferSfd[j], mySystemManager->myBufferSfd[j + 1]);
	}
	mySystemManager->myBufferSfd[j - 1].sfd = -2;
	mySystemManager->myBufferSfd[j - 1].wait = false;
	semGive(semOrder);
	strcpy(mySystemManager->myBufferSfd[j].ipAddr, "");
	if (mySystemManager->getActualSfd() > 0) {
		mySystemManager->requestEvent(REQUEST);
	}
}

bool myConditionTrue() {
	return TRUE;
}

bool myConditionLocalM() {
	return mySystemManager->localMode;
}

bool myConditionChainM() {
	return mySystemManager->chainMode;
}

