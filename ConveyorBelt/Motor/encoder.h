#ifndef ENCODER_H
#define ENCODER_H

#define maxCounterValue 65535

#include "hwFunc.h"
#include "FHV.h"
#include <sysLib.h>
#include "FHV.h"
#include <intLib.h>
#include <hw.h>
#include <time.h>
#include <Kalman.h>
#include <semLib.h>
#include <tickLib.h>
#include <wdLib.h>

class Encoder {
public:
	/* Default constructor */
	Encoder();
	/* Default destructor */
	~Encoder();
	static void myISR(Encoder * myInterruptTest);
	void readRoutine();
	double getTimePulses();
	double getTimePulsesAveraged();
	double getTimePulsesKalman();
	int getPulses();
	int getDirection();

//	struct timespec time_val;
//	long tempTimeUs;
//	long timeDifUs;
//	long timeUsOld;
protected:

private:
	/* instance of objects */
	Kalman * myKalman;
	/* methodes */
	static void wrapper(Encoder * obj);
	static void wdCallingFunctionEnc(Encoder* obj);
	void updateKalman();
	/* variable declaration */
	int counterArray[10];
	int filterCount;
	int filtervalue;
	int tickSet;
	int direction;
	double kalmanValue;
	WDOG_ID wdId;
	SEM_ID sem;
	SEM_ID semKalman;
	unsigned int isrCount;
	unsigned int counterOneValue;
	unsigned int isrCountOverflow;
};

#endif
