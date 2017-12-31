#include "Motor.h"

Motor::Motor() {
	myEncoder = new Encoder;
	getEncoderPulsesZeroCorrected();
	oldDirection = 1;
}

Motor::~Motor() {
	motorOff();
}

/*
 * This function is for finding the right value at speed zero
 * We need this for a smooth zero :)
 */
void Motor::setZero() {
	motorOn();
	writeAnalog(0, (int) faktorStop);
	beLazy(500);
	int actSpeedTest = 0;
	analogueValueatZero = (int) faktorStop;
	int toleranz = 20;
	int counter = 0;
	bool minusPlus = false;
	FOREVER {
		actSpeedTest = getEncoderPulsesZeroCorrected();
		if (actSpeedTest < 0) {
			counter = 0;
			if (actSpeedTest < -toleranz)
				analogueValueatZero -= toleranz;
			else
				analogueValueatZero -= 1;
			writeAnalog(0, analogueValueatZero);
			minusPlus = true;
		} else if (actSpeedTest > 0) {
			counter = 0;
			if (actSpeedTest > toleranz)
				analogueValueatZero += toleranz;
			else
				analogueValueatZero += 1;
			writeAnalog(0, analogueValueatZero);
			minusPlus = false;
		} else if (counter > 100) {
			if (minusPlus) {
				analogueValueatZero -= 1;
				writeAnalog(0, analogueValueatZero);
			} else {
				analogueValueatZero += 1;
				writeAnalog(0, analogueValueatZero);
			}
			break;
		}
		counter++;
		beLazy(15);
	}
	motorOff();
}

void Motor::drive() {
	int temp = analogueValueatZero - (int) actSpeed;
	if (temp > 4095)
		temp = 4095;
	else if (temp < 0)
		temp = 0;
	writeAnalog(0, temp);
}

void Motor::driveLeft() {
	int temp = analogueValueatZero - (int) actSpeed;
	if (temp > 4095)
		temp = 4095;
	else if (temp < 0)
		temp = 0;
	writeAnalog(0, temp);
}
void Motor::driveRight() {
	int temp = analogueValueatZero + (int) actSpeed;
	if (temp > 4095)
		temp = 4095;
	else if (temp < 0)
		temp = 0;
	writeAnalog(0, temp);
}
void Motor::setActSpeed(double actSpeed) {
	this->actSpeed = actSpeed;
//	printf("getspeed: %f\n",actSpeed);
}
double Motor::getActSpeed(bool withDirection) {
//	return (double) getRotationDirection(0) * (double) getEncoderPulsesZeroCorrected() / pulsesPerRevolution;
	return (double) getRotationDirection(0) * myEncoder->getTimePulsesKalman();
}
double Motor::getActSpeed() {
	return myEncoder->getTimePulsesKalman();
//	return myEncoder->getTimePulsesAveraged();
//	return (double) getEncoderPulsesZeroCorrected() / pulsesPerRevolution;
}

int Motor::getDirection() {
	if (oldDirection != myEncoder->getDirection()) {
		oldDirection = myEncoder->getDirection();
		printf("%i\n", myEncoder->getDirection());
	}

	return myEncoder->getDirection();
}

void Motor::testIt() {
	int i = 0;
	int timeCLK = 50;
	double actSpeedRPM = 0.0;
	char pressedKey = 0;
	bool exit = false;
	motorOn();
	FOREVER {
		motorOn();
		FOREVER {
			setActSpeed(200.0);
			driveRight();
			actSpeedRPM = (double) getActSpeed() * 1 / (timeCLK) * 1000 * 60.0;
//			printf("1 actSpeedRPM %.4f\n",actSpeedRPM);
			if (getKey() == 'C') {
				exit = true;
				break;
			}

			if (i++ > 20)
				break;
			beLazy(timeCLK);

		}
		motorOff();
		beLazy(5000);
		motorOn();
		if (exit)
			break;
		i = 0;
		FOREVER {
			setActSpeed(200.0);
			driveLeft();
			actSpeedRPM = (double) getActSpeed() * 1 / (timeCLK) * 1000 * 60.0;
//			printf("2 actSpeedRPM %.4f\n",actSpeedRPM);
			if (getKey() == 'C') {
				exit = true;
				break;
			}
			if (i++ > 20)
				break;
			beLazy(timeCLK);
		}
		if (exit)
			break;
		i = 0;
		motorOff();
	}
	motorOff();
}

int Motor::getZeroSpeedVlaue() {
	return analogueValueatZero;
}
