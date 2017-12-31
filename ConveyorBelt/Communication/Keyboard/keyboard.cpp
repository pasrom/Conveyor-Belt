#include <stdio.h>
#include <sysLib.h>
#include <stdLib.h>
#include "keyboard.h"
#include "hwFunc.h"
#include "dataTypes.h"

int keyCount;

Keyboard::Keyboard(SystemManager *systemManager) {
	printf("Keyboard Konstruktor!\n\r");
	myInSystemManager = systemManager;
	myParser = new Parser;
	myInKeyboard = this;
	keyPressedOld = false;
	deleteTask = false;
	return;
}

Keyboard::~Keyboard() {
	pressedKey = 0;
	printf("Keyboard Dekonstruktor\n");
	taskDelete(taskID);
	return;
}

void Keyboard::initTask() {
	taskID = taskSpawn("tKeyboard", 110, VX_FP_TASK, 0x1000, (FUNCPTR) Keyboard::KeyboardWorkTask, (int) myInKeyboard, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

void Keyboard::deinitTask() {
	deleteTask = true;
	printf("deleteTask %i\n", deleteTask);
}

void Keyboard::KeyboardWorkTask(Keyboard * obj) {
	Keyboard* myself = (Keyboard*) obj;
	myself->run();
}

void Keyboard::parseComannd() {
	/* do a simple char to string conversion... */
	char tempStr[4] = "";
	tempStr[0] = pressedKey;
	tempStr[1] = '\0';
	Cmd temp = myParser->parseCmd(tempStr, KEYBOARD);
	printf("pressed Key %s, cmd %i\n", tempStr, temp);
	myInSystemManager->requestEvent(temp);
}

void Keyboard::checkPressedKey() {
	pressedKey = getKey();
	if ((pressedKey != 0) && !keyPressedOld) {
		keyPressedOld = true;
		parseComannd();
	} else if (pressedKey == 0 && keyPressedOld) {
		keyPressedOld = false;
	}
}

void Keyboard::run() {
	FOREVER {
		checkPressedKey();
		beLazy(80);
		if (deleteTask) {
			printf("Keyboard delete\n");
			break;
		}
	}
}
