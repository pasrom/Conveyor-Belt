#ifndef DISPLAY_H_
#define DISPLAY_H_
#include "systemManager.h"
#include "disp.h"
#include "FHV.h"
#include <sysLib.h>
#include <stdLib.h>
#include <string>
#include "stateMachine.h"
#include <time.h>

class SystemManager;
class StateMachine;

class Display {

public:
	Display(SystemManager *systemManager, StateMachine *stateMachine);
	~Display();
	void initTask();
	void deinitTask();
	void setOperationMode(char *str);
	void setMaxSpeed(int speed);
	void setDirection(bool dir);
	void setStatus(char *str);
	void setInitialization(char *str);
	void setRecTimestamp(struct timespec *time);
	void setSendTimestamp(struct timespec *time);
	void setActualMotorSpeed(double actSpeed);
	void setSetSpeed(double setSpeed);
	char operationMode[60];
	int maxSpeed;
	bool direction;
	char status[60];
	char initialization[60];
	timespec recTimestamp;
	timespec sendTimestamp;
private:
	SystemManager *mySystemManager;
	Parser * myParser;
	Display * myInDisplay;
	StateMachine * myInStateMachine;

	void update();
	void writeOperationMode();
	void writeMaxSpeed();
	void writeDirection();
	void writeStatus();
	void writeInitialization();
	void writeRecTimestamp();
	void writeSendTimestamp();
	void writeActualMotorSpeed();
	void writeSetSpeed();
	static void DisplayWorkTask(Display * obj);
	void run();

	double actSpeed;
	double setSpeed;
	int taskID;
	int rowCounter;
	bool deleteTask;
};

#endif
