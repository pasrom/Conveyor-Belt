#ifndef MOTOR_H
#define MOTOR_H

#include "hwFunc.h"
#include "FHV.h"
#include <sysLib.h>
#include "encoder.h"
#define umrechnungsfaktor (5.0/4096.0)
#define faktorStop (4096.0/2.0)

#define pulsesPerRevolution (64.0)
class Encoder;

class Motor {
public:
	/* Default constructor */
	Motor();
	/* Default destructor */
	~Motor();

	void drive();
	void driveLeft();
	void driveRight();
	void setActSpeed(double actSpeed);
	double getActSpeed();
	double getActSpeed(bool withDirection);
	int getZeroSpeedVlaue();
	int getDirection();
	void setZero();
	void testIt();
protected:

private:
	/* instance of objects */
	Encoder * myEncoder;

	double actSpeed;
	int analogueValueatZero;
	int zeroSpeed;
	int oldDirection;
	/* methodes */

	/* variable declaration */

};

#endif
