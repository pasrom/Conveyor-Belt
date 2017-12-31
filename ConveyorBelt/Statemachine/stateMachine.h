
#ifndef STATEMACHINE_H_
#define STATEMACHINE_H_

#include <queue>
#include <semLib.h>
#include "diaTimer.h"
#include "stateTable.h"

//StateMachine * myStateMachine;

class StateMachine {
public:
	StateMachine();
	~StateMachine();
	void sendEvent(std::string myEvent);
	void runToCompletion();
	std::string myEvent;
	DiaTimer * diaTimerTable[MAXDIA];
//	char * getActualEvent();
private:
	SEM_ID volatile semQueue, semEvent;
	void putEvent(std::string myEvent);
	std::string getEvent();
	std::queue<std::string> queue;
	std::string actualEvent;
};

#endif // STATEMACHINE_H_