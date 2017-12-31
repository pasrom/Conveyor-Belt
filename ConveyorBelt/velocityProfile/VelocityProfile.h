#ifndef VELOCITY_PROFILE_H
#define VELOCITY_PROFILE_H

#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <assert.h>

#include "systemManager.h"

class VelocityProfile {
private:
	int t_r;
	int t_f;
	int t_t;
	int a;
	bool direction;
public:
	VelocityProfile(int t_r, int t_f, int t_t, int a, bool direction);
	void setT_r(int T_r);
	void setT_f(int T_f);
	void setT_t(int T_t);
	void setA(int a);
	void setDirection(bool direction);
	int getT_r();
	int getT_f();
	int getT_t();
	int getA();
	bool getDirection();
};
#endif
