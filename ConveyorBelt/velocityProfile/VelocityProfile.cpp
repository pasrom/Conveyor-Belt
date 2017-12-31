#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <assert.h>

#include "VelocityProfile.h"

VelocityProfile::VelocityProfile(int t_r, int t_f, int t_t, int a, bool direction) {
	this->t_r = t_r;
	this->t_f = t_f;
	this->t_t = t_t;
	this->a = a;
	this->direction = direction;
}

void VelocityProfile::setT_r(int T_r) {
	this->t_r = T_r;
}

void VelocityProfile::setT_f(int T_f) {
	this->t_f = T_f;
}

void VelocityProfile::setT_t(int T_t) {
	this->t_t = T_t;
}

void VelocityProfile::setA(int a) {
	this->a = a;
}

void VelocityProfile::setDirection(bool direction) {
	this->direction = direction;
}

int VelocityProfile::getT_r() {
	return t_r;
}

int VelocityProfile::getT_f() {
	return t_f;
}

int VelocityProfile::getT_t() {
	return t_t;
}

int VelocityProfile::getA() {
	return a;
}

bool VelocityProfile::getDirection() {
	return direction;
}
