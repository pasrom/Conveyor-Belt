#include "Display.h"

Display::Display(SystemManager *systemManager, StateMachine *stateMachine) {
	mySystemManager = systemManager;
	myInStateMachine = stateMachine;
	myInDisplay = this;
	deleteTask = false;
	rowCounter = 0;

}

Display::~Display() {
	//delete mySystemManager;
	//delete myInStateMachine;
	//delete myInDisplay;
	taskDelete(taskID);
	printf("Display Dekonstruktor\n");
	return;
}

void Display::initTask() {
	setMaxSpeed(1800);
	setDirection(false);
	char str[] = "not set";
	setOperationMode(str);
	setStatus(str);
	setInitialization(str);
	recTimestamp.tv_sec = time(NULL);
	recTimestamp.tv_nsec = 0;
	sendTimestamp.tv_sec = time(NULL);
	sendTimestamp.tv_nsec = 0;
	setSetSpeed(0.0);
	setActualMotorSpeed(0.0);
	printf("%i\n", maxSpeed);
	taskID = taskSpawn("tDisplay", 110, VX_FP_TASK, 0x10000, (FUNCPTR) Display::DisplayWorkTask, (int) myInDisplay, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return;
}
void Display::deinitTask() {
	deleteTask = true;
	printf("Display deinitTasks\n");
}

void Display::run() {
	FOREVER {
		update();
		beLazy(50);
		if (deleteTask)
			break;
	}
}

void Display::DisplayWorkTask(Display * obj) {
	Display* myself = (Display*) obj;
	myself->run();
}

void Display::update() {
	rowCounter = 0;
	char str[254] = "";
	struct tm *timeB;
	struct timespec time_val;
	clock_gettime(CLOCK_REALTIME, &time_val);
	timeB = localtime(&time_val.tv_sec);
	sprintf(str, "%04d.%02d.%02d. %02d:%02d:%02d", timeB->tm_year, timeB->tm_mon, timeB->tm_mday, timeB->tm_hour, timeB->tm_min, timeB->tm_sec);
	writeToDisplay(rowCounter, 1, "I am a conveyor belt :)");
	writeToDisplay(rowCounter++, 30, str);
	writeOperationMode();
	writeMaxSpeed();
	writeDirection();
	writeStatus();
	writeInitialization();
	writeRecTimestamp();
	writeSendTimestamp();
	writeSetSpeed();
	writeActualMotorSpeed();

}
void Display::writeOperationMode() {
	writeToDisplay(rowCounter, 1, "Betriebsmodus..............                    ");
	writeToDisplay(rowCounter++, 25, operationMode);
}
void Display::writeMaxSpeed() {
	char str[20] = "";
	writeToDisplay(rowCounter, 1, "max. Profildrehzahl........                    ");
	sprintf(str, "%i", maxSpeed);
	writeToDisplay(rowCounter++, 25, str);
}
void Display::writeDirection() {
	char str[20] = "";
	writeToDisplay(rowCounter, 1, "Richtung...................                    ");
	direction ? sprintf(str, "left") : sprintf(str, "right");
	writeToDisplay(rowCounter++, 25, str);
}
void Display::writeStatus() {
	char str[20] = "";
	writeToDisplay(rowCounter, 1, "Status.....................                    ");
	writeToDisplay(rowCounter++, 25, status);
}
void Display::writeInitialization() {
	char str[20] = "";
	writeToDisplay(rowCounter, 1, "Initialisierung............                    ");
	writeToDisplay(rowCounter++, 25, initialization);
}
void Display::writeRecTimestamp() {
	char str[254] = "";
	struct tm *timeB;
	writeToDisplay(rowCounter, 1, "Sendezeitstempel...........                    ");
	timeB = localtime(&recTimestamp.tv_sec);
	sprintf(str, "%04d.%02d.%02d. %02d:%02d:%02d", timeB->tm_year, timeB->tm_mon, timeB->tm_mday, timeB->tm_hour, timeB->tm_min, timeB->tm_sec);
	writeToDisplay(rowCounter++, 25, str);
}
void Display::writeSendTimestamp() {
	char str[254] = "";
	struct tm *timeB;
	writeToDisplay(rowCounter, 1, "Empfangszeitstempel........                    ");
	timeB = localtime(&sendTimestamp.tv_sec);
	sprintf(str, "%04d.%02d.%02d. %02d:%02d:%02d", timeB->tm_year, timeB->tm_mon, timeB->tm_mday, timeB->tm_hour, timeB->tm_min, timeB->tm_sec);
	writeToDisplay(rowCounter++, 25, str);
}

void Display::writeActualMotorSpeed() {
	char str[254] = "";
	writeToDisplay(rowCounter, 1, "Aktuelle Drehzahl........                       ");
	sprintf(str, "%i", (int) actSpeed);
	writeToDisplay(rowCounter++, 25, str);
}
void Display::writeSetSpeed() {
	char str[254] = "";
	writeToDisplay(rowCounter, 1, "Solldrehzahl.............                       ");
	sprintf(str, "%i", (int) setSpeed);
	writeToDisplay(rowCounter++, 25, str);
}

void Display::setOperationMode(char *str) {
	sprintf(operationMode, "%s", str);
}
void Display::setMaxSpeed(int speed) {
	maxSpeed = speed;
}
void Display::setDirection(bool dir) {
	direction = dir;
}
void Display::setStatus(char *str) {
	sprintf(status, "%s", str);
}
void Display::setInitialization(char *str) {
	sprintf(initialization, "%s", str);
}
void Display::setRecTimestamp(struct timespec *time) {
	recTimestamp.tv_sec = time->tv_sec;
	recTimestamp.tv_nsec = time->tv_nsec;
}
void Display::setSendTimestamp(struct timespec *time) {
	sendTimestamp.tv_sec = time->tv_sec;
	sendTimestamp.tv_nsec = time->tv_nsec;
}
void Display::setActualMotorSpeed(double actSpeed) {
	this->actSpeed = actSpeed;
}

void Display::setSetSpeed(double setSpeed) {
	this->setSpeed = setSpeed;
}
