#ifndef CONTROLLER_H
#define CONTROLLER_H

/* ANSI C headers */
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* VxWorks headers */
#include <vxWorks.h>
#include <taskLib.h>
#include <sysLib.h>
#include <semLib.h>
#include <rebootLib.h>
#include <logLib.h>
#include <tickLib.h>

#include "rtwtypes.h"
#include "Subsystem_private.h"
#include "Subsystem_types.h"
#include "Subsystem.h"

#include "dataTypes.h"

/* this sets the standard stack size for spawned tasks used by the model.
 * this can be changed by compiling with '-DSTACK_SIZE=nnn' where nnn is
 * the stack size desired.
 */
#ifndef STACK_SIZE
#define STACK_SIZE                     16384
#endif

#include "systemManager.h"
#include "VelocityProfile.h"
#include "Motor.h"
#include "encoder.h"

class SystemManager;
class VelocityProfile;
class Motor;

class Controller {
public:
	/* Default constructor */
	Controller(SystemManager *systemManager, StateMachine * stateMachine);
	/* Default destructor */
	~Controller();

	void controllerStart();
	static void controllerWrapper(Controller* obj);
	static void controllerTask(Controller* obj);
	void controllerRun();
	void setProfile(VelocityProfile * velocityProfile);
	int_T tBaseRate();

	void Subsystem_step();
	void Subsystem_initialize();
	void Subsystem_terminate();
	void setRampUp();
	void setRampDown();
	void setConstantVelocity();
	void inputController();
	void outputController();
	void setSpeedToZero();
	static Parameters_Subsystem Subsystem_P;
protected:

private:
	/* instance of objects */
	SystemManager * myInSystemManager;
	VelocityProfile * myInVelocityProfile;
	StateMachine * myInStateMachine;
	Motor * myMotor;
	/* methodes */

	/* variable declaration */
	real_T requestedSR, actualSR;
	int_T VxWorksTIDs[1];
	SEM_ID rtTaskSemaphoreList[1];
	SEM_ID rtClockSem, startStopSem;
	/* Block states (auto storage) */

	D_Work_Subsystem Subsystem_DWork;

	/* External inputs (root inport signals with auto storage) */
	ExternalInputs_Subsystem Subsystem_U;

	/* External outputs (root outports fed by signals with auto storage) */
	ExternalOutputs_Subsystem Subsystem_Y;

	int count;
	int tickSet;
	int counterSetZero;
	double timeCLK;
	bool motorIsOn;
	WDOG_ID testWD;
	static void wdCallingFunction(Controller* obj);
	double actSpeedRPM;
	double inputSpeedRPM;
	double delayToStart;
	profileState myProfileState;
	int countMax;
	int sollV[1000];
	int istV[1000];
	int countResponse;
};
#endif
