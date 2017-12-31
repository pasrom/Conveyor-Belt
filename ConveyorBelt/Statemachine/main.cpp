/*! \mainpage This is the documentaton of the conveyor belt
 *
 * \section intro_sec Introduction
 *
 * This document contains the documentation 
 *
 * \section install_sec Installation
 *
 * \subsection step1 Step 1: Opening the box
 *
 * etc...
 */
#include <taskLib.h>
#include <stdio.h>
#include <intLib.h>
#include <sysLib.h>
#include <stdLib.h>
#include "ifLib.h"
#include "systemManager.h"
#include "stateMachine.h"
#include "hwFunc.h"
#include <time.h>
#include "Motor.h"

#include "encoder.h"

//#define __COM
StateMachine * myStateMachine;
SystemManager * mySystemManager;
Encoder * myInterruptTest;

int main(void) {
	struct timespec time_val;
	// Set tick to 1 ms. This is the time resolution!
	double tickMs = 0.001;
	sysClkRateSet((int) (1.0 / tickMs));

	// Create instance of top class
	mySystemManager = new SystemManager;

	// Start the state machine. This method blocks, so no while(1) is needed.
	myStateMachine->sendEvent("InitSystem");
	myStateMachine->runToCompletion();
}

