#include "Controller.h"

Controller::Controller(SystemManager *systemManager, StateMachine *stateMachine) {
	myInSystemManager = systemManager;
	myInStateMachine = stateMachine;

	/* lets do this shit with a realy cool watchdog :)*/
	timeCLK = Subsystem_P.Integrator_gainval * 1000.0; /* put in here m seconds!!!*/
	tickSet = (int) ((double) sysClkRateGet() * timeCLK / 1000.0);
	if (tickSet == 0) {
		perror("tickSet too small, correct it!");
		tickSet = 1;
		timeCLK = (double) tickSet / ((double) sysClkRateGet()) * 1000.0;
		Subsystem_P.Integrator_gainval = timeCLK / 1000.0;
	}
	testWD = wdCreate();

	printf("tickSet %i sysClkRateGet %i timeCLK %f\n", tickSet, sysClkRateGet(), timeCLK);
	myMotor = new Motor;
	myMotor->setZero();

	rtClockSem = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY);
	startStopSem = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY);
	inputSpeedRPM = 0.0;
	actSpeedRPM = 0.0;
	motorIsOn = false;
	delayToStart = 0.0;
	myProfileState = STOPMOTOR;
	countMax = 0;
	count = 0;
	counterSetZero = 0;
	countResponse = 0;
	/* Block parameters (auto storage) */
	return;
}

Controller::~Controller() {

}

void Controller::controllerStart() {
	int priority = 50;

	printf("Warning: The simulation will run forever. "
			"To change this behavior select the 'MAT-file logging' option.\n");
	if (priority <= 0 || priority > 255 - (1) + 1) {
		priority = 30;
	}
	/* Initialize model */
	Subsystem_initialize();

	VxWorksTIDs[0] = taskSpawn("tBaseRate", priority, VX_FP_TASK, STACK_SIZE, (FUNCPTR) Controller::controllerWrapper, (int) this, 0, 0, 0, 0, 0, 0,
			0, 0, 0);
	if (wdStart(testWD, tickSet, (FUNCPTR) Controller::wdCallingFunction, (int) this) >= 0)
		printf("\nSimulation starting\n");

	semTake(startStopSem, WAIT_FOREVER);
	wdDelete(testWD);
	taskDelete(VxWorksTIDs[0]);
	semDelete(rtClockSem);
	semDelete(startStopSem);

	/* Terminate model */
	Subsystem_terminate();
}

void Controller::controllerWrapper(Controller* obj) {
	Controller * mySelf = (Controller *) obj;
	mySelf->tBaseRate();
}

void Controller::controllerTask(Controller* obj) {
	Controller * mySelf = (Controller *) obj;
	mySelf->controllerStart();
}

void Controller::controllerRun() {
	if (taskSpawn("tController", 150, VX_FP_TASK, 10000, (FUNCPTR) Controller::controllerTask, (int) this, 0, 0, 0, 0, 0, 0, 0, 0, 0) < 0) {
		perror("failed to start the controler task for the Controller");
	}
}

void Controller::setProfile(VelocityProfile * velocityProfile) {
	myInVelocityProfile = velocityProfile;
	/*init the velocity loging*/
	for (int i = 0; i < sizeof(sollV) / sizeof(sollV[0]); ++i) {
		sollV[i] = 0;
		istV[i] = 0;
	}
}

int_T Controller::tBaseRate() {
	int_T i;
	while (1) {
		if (semTake(rtClockSem, NO_WAIT) != ERROR) {
			logMsg("Rate for SingleRate task too fast.\n", 0, 0, 0, 0, 0, 0);
		} else {
			semTake(rtClockSem, WAIT_FOREVER);
		}
		/* Set model inputs here */
		actSpeedRPM = (double) myMotor->getDirection() * myMotor->getActSpeed();
		inputController();
		/* Step the model */
		if (myProfileState != STOPMOTOR) {
			Subsystem_step();
		}
		/* Get model outputs here */
		outputController();
		myInSystemManager->myDisplay->setSetSpeed(inputSpeedRPM);
		myInSystemManager->myDisplay->setActualMotorSpeed(actSpeedRPM);
	}

	return (1);
}

/* Model step function */
void Controller::Subsystem_step(void) {
	real_T rtb_Sum;
	real_T rtb_Saturation;

	/* Sum: '<S2>/Sum' incorporates:
	 *  DiscreteIntegrator: '<S2>/Integrator'
	 *  Gain: '<S2>/Proportional Gain'
	 *  Inport: '<Root>/In2'
	 */
	rtb_Sum = Subsystem_P.ProportionalGain_Gain * Subsystem_U.In2 + Subsystem_DWork.Integrator_DSTATE;
	/* Saturate: '<S2>/Saturation' */
	if (rtb_Sum > Subsystem_P.Saturation_UpperSat) {
		rtb_Saturation = Subsystem_P.Saturation_UpperSat;
	} else if (rtb_Sum < Subsystem_P.Saturation_LowerSat) {
		rtb_Saturation = Subsystem_P.Saturation_LowerSat;
	} else {
		rtb_Saturation = rtb_Sum;
	}

	/* End of Saturate: '<S2>/Saturation' */

	/* Outport: '<Root>/u_ref' */
	Subsystem_Y.u_ref = rtb_Saturation;

	/* Update for DiscreteIntegrator: '<S2>/Integrator' incorporates:
	 *  Gain: '<S2>/Integral Gain'
	 *  Gain: '<S2>/Kb'
	 *  Inport: '<Root>/In2'
	 *  Sum: '<S2>/SumI1'
	 *  Sum: '<S2>/SumI2'
	 */
	Subsystem_DWork.Integrator_DSTATE += ((rtb_Saturation - rtb_Sum) * Subsystem_P.Kb_Gain + Subsystem_P.IntegralGain_Gain * Subsystem_U.In2)
			* Subsystem_P.Integrator_gainval;
}

void Controller::wdCallingFunction(Controller* obj) {
	Controller * mySelf = (Controller *) obj;
	semGive(mySelf->rtClockSem);
	wdStart(mySelf->testWD, mySelf->tickSet, (FUNCPTR) Controller::wdCallingFunction, (int) obj);
}

/* Model initialize function */
void Controller::Subsystem_initialize(void) {
	/* Registration code */

	/* states (dwork) */
	Subsystem_DWork.Integrator_DSTATE = 0.0;

	/* external inputs */
	Subsystem_U.In2 = 0.0;

	/* external outputs */
	Subsystem_Y.u_ref = 0.0;

	/* InitializeConditions for DiscreteIntegrator: '<S2>/Integrator' */
	Subsystem_DWork.Integrator_DSTATE = Subsystem_P.Integrator_IC;
}

/* Model terminate function */
void Controller::Subsystem_terminate(void) {
	/* (no terminate code required) */
}
void Controller::setRampUp() {
	count = 0;
	countResponse = 0;
	delayToStart = 0.0;
	/* external inputs */
	Subsystem_U.In2 = 0.0;

	/* external outputs */
	Subsystem_Y.u_ref = 0.0;
	myProfileState = RAMPUP;
}
void Controller::setRampDown() {
	count = 0;
	delayToStart = 0.0;
	/* external inputs */
	Subsystem_U.In2 = 0.0;

	/* external outputs */
	Subsystem_Y.u_ref = 0.0;
	myProfileState = RAMPDOWN;
}
void Controller::setConstantVelocity() {
	count = 0;
	delayToStart = 0.0;
	/* external inputs */
	Subsystem_U.In2 = 0.0;

	/* external outputs */
	Subsystem_Y.u_ref = 0.0;
	myProfileState = CONSTANTVELOCITY;
}

void Controller::setSpeedToZero() {
	inputSpeedRPM = 0.0;
	motorOff();
	myProfileState = STOPMOTOR;
	motorIsOn = false;
	delayToStart = 0.0;
	countResponse = 0;
	/* external inputs */
	Subsystem_U.In2 = 0.0;

	/* external outputs */
	Subsystem_Y.u_ref = 0.0;
	/* InitializeConditions for DiscreteIntegrator: '<S2>/Integrator' */
	Subsystem_DWork.Integrator_DSTATE = Subsystem_P.Integrator_IC;
}

void Controller::inputController() {
	if (motorIsOn) {
		switch (myProfileState) {
		case RAMPUP:
			countMax = (int) ((double) myInVelocityProfile->getT_r() / timeCLK);
			if (count <= countMax) {
				inputSpeedRPM = (myInVelocityProfile->getA() / (myInVelocityProfile->getT_r() / (timeCLK))) * count;
				count++;
				countResponse++;
			} else {
				myProfileState = STOPMOTOR;
				myInStateMachine->sendEvent("rampupf");
			}
			break;
		case RAMPDOWN:
			countMax = (int) ((double) myInVelocityProfile->getT_f() / timeCLK);
			if (count <= countMax) {
				inputSpeedRPM = (myInVelocityProfile->getA() - (myInVelocityProfile->getA() / (myInVelocityProfile->getT_f() / timeCLK)) * count);
				count++;
				countResponse++;
			} else {
				inputSpeedRPM = 0.0;
				countResponse++;
				counterSetZero++;
				if (abs(actSpeedRPM) < 1.0 || (double)counterSetZero *Subsystem_P.Integrator_gainval > 0.15) {
					myProfileState = STOPMOTOR;
					myInStateMachine->sendEvent("rampdownf");
					counterSetZero = 0;
				}
			}
			break;
		case CONSTANTVELOCITY:
			countMax = (int) ((double) myInVelocityProfile->getT_t() / timeCLK);
			if (count <= countMax) {
				inputSpeedRPM = myInVelocityProfile->getA();
				count++;
				countResponse++;
			} else {
				myProfileState = STOPMOTOR;
				myInStateMachine->sendEvent("constf");
			}
			break;
		case STOPMOTOR:
			setSpeedToZero();
			break;
		default:
			setSpeedToZero();
			break;
		}
		/* set the profile to the correct direction */
		if (myInVelocityProfile->getDirection())
			inputSpeedRPM *= -1.0;
		/* calculate difference from actual speed and input */
		Subsystem_U.In2 = inputSpeedRPM - actSpeedRPM;
		sollV[countResponse] = (int) inputSpeedRPM;
		istV[countResponse] = (int) actSpeedRPM;
	}
	if ((myProfileState != STOPMOTOR) && !motorIsOn) {
		motorOn();
		if (delayToStart > 50.0) {
			motorIsOn = true;
		}
		delayToStart += timeCLK;
	}
}

void Controller::outputController() {
	/*
	 * 4095/2 = 2047,5 -> Midpoint
	 * 2047,5 are equal to 12 V
	 * therfore the euqtion is
	 * tempOutSpeed = 4095/2 * 1 / 12 V * Subsystem_Y.u_ref;
	 */
	double tempOutSpeed = 4095.0 / (2.0 * 12.0) * Subsystem_Y.u_ref;

	myMotor->setActSpeed(tempOutSpeed);
	myMotor->drive();
}
/* alte Regelparameter */
//Parameters_Subsystem Controller::Subsystem_P = { 0.015625, /* Expression: I
// * Referenced by: '<S2>/Integral Gain'
// */
//0.015, /* Computed Parameter: Integrator_gainval 0.015625
// * Referenced by: '<S2>/Integrator'
// */
//0.0, /* Expression: InitialConditionForIntegrator
// * Referenced by: '<S2>/Integrator'
// */
//8.0E-6, /* Expression: P
// * Referenced by: '<S2>/Proportional Gain'
// */
//18.0, /* Expression: UpperSaturationLimit
// * Referenced by: '<S2>/Saturation'
// */
//-18.0, /* Expression: LowerSaturationLimit
// * Referenced by: '<S2>/Saturation'
// */
//0.015625 /* Expression: Kb
// * Referenced by: '<S2>/Kb'
// */
//};
/* neue Regelparameter */
Parameters_Subsystem Controller::Subsystem_P = { 0.0351, /* Expression: I K_in
 * Referenced by: '<S2>/Integral Gain'
 */
0.015, /* Computed Parameter: Integrator_gainval 0.015625
 * Referenced by: '<S2>/Integrator'
 */
0.0, /* Expression: InitialConditionForIntegrator
 * Referenced by: '<S2>/Integrator'
 */
0.00048, /* Expression: P				Kp_n
 * Referenced by: '<S2>/Proportional Gain'
 */
18.0, /* Expression: UpperSaturationLimit
 * Referenced by: '<S2>/Saturation'
 */
-18.0, /* Expression: LowerSaturationLimit
 * Referenced by: '<S2>/Saturation'
 */
0.0351 /* Expression: Kb 			K_in
 * Referenced by: '<S2>/Kb'
 */
};
