#ifndef KEYBOARD_H_
#define KEYBOARD_H_
#include "systemManager.h"
#include "parser.h"

class SystemManager;
//SystemManager *mySystemManager;
class Keyboard {

public:
	Keyboard(SystemManager *systemManager);
	~Keyboard();
	void checkPressedKey();
	void initTask();
	void deinitTask();
	SystemManager *myInSystemManager;
private:

	Parser * myParser;
	Keyboard * myInKeyboard;
	void parseComannd();
	void run();
	static void KeyboardWorkTask(Keyboard * obj);
	char pressedKey;
	int taskID;
	bool keyPressedOld;
	bool deleteTask;
};

#endif // KEYBOARD_H_