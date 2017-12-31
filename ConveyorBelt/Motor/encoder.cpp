#include "encoder.h"

Encoder::Encoder() {
	/* interrupt verbinden myISR anpassen*/
	intConnect(INUM_TO_IVEC(aioIntNum), (VOIDFUNCPTR) Encoder::myISR, (int) this);
	sysIntEnablePIC(aioIRQNum);

	/* aktivieren von change of state port c 	*/
	sysOutByte(aioBase + intEnAddress,
	intAllEnBit + intPortC0Bit + intCntEnBit);
	isrCount = 0;
	filterCount = 0;
	counterOneValue = 9999;
	isrCountOverflow = 0;
	filtervalue = 1.0;
	printf("%i\n", sizeof(counterArray));
	int k;
	for (k = 0; k < filterCount > sizeof(counterArray) / sizeof(counterArray[0]); ++k) {
		counterArray[k] = 0;
	}
	/* only for tests ... */
//	initHardware(0);
//	writeAnalog(0, 4095 / 3);
//	motorOn();
	/* counter einschalten */
	sysOutByte(aioBase + cntCntrlReg, cnt1Sel + bothByteSel + cntMode4);
	sysOutByte(aioBase + cnt1Address, 0xFF);
	sysOutByte(aioBase + cnt1Address, 0xFF);

	/* kalman things (q, r, p, x)*/
	myKalman = new Kalman(0.0625, 0.5, 1000.0, 0.0);
	kalmanValue = 0.0;
	tickSet = (int) ((double) sysClkRateGet() * 5.0 / 1000.0);
	wdId = wdCreate();
	sem = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY);
	semKalman = semMCreate(SEM_Q_PRIORITY);
	taskSpawn("tEncoder", 90, VX_FP_TASK, 10000, (FUNCPTR) Encoder::wrapper, (int) this, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	wdStart(wdId, tickSet, (FUNCPTR) Encoder::wdCallingFunctionEnc, (int) this);
}

Encoder::~Encoder() {

}
void Encoder::wrapper(Encoder * obj) {
	Encoder * myself = (Encoder *) obj;
	myself->updateKalman();
}

void Encoder::updateKalman() {
	FOREVER {
		if (semTake(sem, NO_WAIT) != ERROR) {
			logMsg("Rate for tEncoder task too fast.\n", 0, 0, 0, 0, 0, 0);
		} else {
			semTake(sem, WAIT_FOREVER);
		}
		double temp = myKalman->getFilteredValue((double) counterOneValue);
		semTake(semKalman, WAIT_FOREVER);
		kalmanValue = temp;
		semGive(semKalman);
	}
}

double Encoder::getTimePulses() {
	return filtervalue / (double) counterOneValue / 64.0 * 60.0 * 1000000.0;
}

double Encoder::getTimePulsesAveraged() {
	int k;
	double temp;

	for (k = 0; k < sizeof(counterArray) / sizeof(counterArray[0]); ++k) {
		temp += (double) counterArray[k];
	}
	temp = temp / (double) k;
	return 1.0 / temp / 64.0 * 60.0 * 1000000.0;
}

double Encoder::getTimePulsesKalman() {
	double temp = 1.0 / 64.0 * 60.0 * 1000000.0;
	semTake(semKalman, WAIT_FOREVER);
	temp = temp / kalmanValue;
	semGive(semKalman);
	return temp;
}

int Encoder::getPulses() {
	int temp = isrCount;
	isrCount = 0;
	return temp;
}

void Encoder::readRoutine() {
	unsigned short statusByte, temp;
	/* work around to get the rotation speed nearly to zero*/
	counterOneValue = 100 * maxCounterValue;
	/* hier wird geschaut ob der interrupt vom counter kommt*/
	statusByte = sysInByte(aioBase + intEnAddress);
	temp = statusByte & intCntEnBit;
	if (temp != 0) {
		isrCountOverflow++;
		sysOutByte(aioBase + intClearAddress, intCntEnBit); //Interrupt loeschen
	}
	/* keine Bewegung innerhalb ca. 6,5 sec -> reset counter */
	if (isrCountOverflow > 100) {
		isrCountOverflow = 0;
		int k;
		for (k = 0; k < filterCount > sizeof(counterArray) / sizeof(counterArray[0]); ++k) {
			counterArray[k] = 0;
		}
	}
	/* hier wird geschaut ob der interrupt vom drehgeber kommt*/
	temp = statusByte & intPortC0Bit;
	if (temp != 0) {
		isrCount++;

		/* latch the counter in order to read it */
		sysOutByte(aioBase + cntCntrlReg, cnt1Sel + latchSel);
		/* read the counter value and calculate rotation speed */
		unsigned short temp = sysInByte(aioBase + cnt1Address);
		/* maxCounterValue - actualCounterValue + OverflowCount*maxCounterValue */
		counterOneValue =
		maxCounterValue - (temp + 256 * sysInByte(aioBase + cnt1Address)) + isrCountOverflow * maxCounterValue;
		sysOutByte(aioBase + intClearAddress, intPortC0Bit); //Interrupt loeschen
		direction = getRotationDirection(0);
		/* start counter overflow detection from begining*/
		isrCountOverflow = 0;
		counterArray[filterCount] = counterOneValue;
		if (filterCount > sizeof(counterArray) / sizeof(counterArray[0])) {
			filterCount = 0;
		} else {
			filterCount++;
		}
	}
	/* counter mode einstellen und auf maximale Zeit */
	sysOutByte(aioBase + cntCntrlReg, cnt1Sel + bothByteSel + cntMode4);
	sysOutByte(aioBase + cnt1Address, 0xFF);
	sysOutByte(aioBase + cnt1Address, 0xFF);
}

int Encoder::getDirection() {
	return direction;
}

void Encoder::myISR(Encoder * myInterruptTest) {
	Encoder * mySelf = (Encoder *) myInterruptTest;
	mySelf->readRoutine();

}

void Encoder::wdCallingFunctionEnc(Encoder* obj) {
	Encoder * mySelf = (Encoder *) obj;
	semGive(mySelf->sem);
	wdStart(mySelf->wdId, mySelf->tickSet, (FUNCPTR) Encoder::wdCallingFunctionEnc, (int) obj);
}
