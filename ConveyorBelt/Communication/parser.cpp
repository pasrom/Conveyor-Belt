//
//  handler.cpp
//  TestConveyorBelt
//
//  Created by roman passler on 22.04.17.
//  Copyright © 2017 roman passler. All rights reserved.
//

#include "parser.h"

Parser::Parser() {
}
Parser::~Parser() {
}

parseCommand Parser::definedCmd[] = { { REQUEST, "Request\r\n", SERVER_CLIENT }, { READY, "Ready\r\n", SERVER_CLIENT }, { WAIT, "Wait\r\n",
		SERVER_CLIENT }, { RELEASE, "Release\r\n", SERVER_CLIENT }, { PLUS, "plus", TELNET }, { MINUS, "minus", TELNET }, { STOP, "stop", TELNET }, {
		CHAIN, "chain", TELNET }, { LOCALM, "local", TELNET }, { LEFT, "left", TELNET }, { RIGHT, "right", TELNET }, { PLUS, "2", KEYBOARD }, { MINUS,
		"8", KEYBOARD }, { STOP, "C", KEYBOARD }, { CHAIN, "E", KEYBOARD }, { LOCALM, "F", KEYBOARD }, { LEFT, "4", KEYBOARD },
		{ RIGHT, "5", KEYBOARD }, { DELETE, "9", KEYBOARD }, { FINISHED, "finished", CONTROLLER }, { NOTFOUND, "NOTFOUND", ALL } };

Cmd Parser::parseCmd(char *str, Source sollEn) {
	int i = 0;
	int end = 0;
	if (SERVER_CLIENT == sollEn)
		end = -2; /* edit this, if something happens with \r\n */
	Cmd temp = NOTFOUND;
	int StrLength = strlen(str);
	for (i = 0; i < sizeof(definedCmd) / sizeof(parseCommand); ++i) {
		int k = 0;
		if (definedCmd[i].sourceCb == sollEn && strncmp(str, definedCmd[i].cmdString, strlen(definedCmd[i].cmdString) + end) == 0) {
			printf("sizeof %i\n", strlen(definedCmd[i].cmdString));
			temp = definedCmd[i].commandEnum;
			break;
		}
	}
	return temp;
}

const char* Parser::parseCmd(Cmd cmd) {
	int i = 0;
	static char temp[50] = "NOTFOUND\r\n";
	for (i = 0; i < sizeof(definedCmd) / sizeof(parseCommand); ++i) {
		if (definedCmd[i].commandEnum == cmd) {
			sprintf(temp, "%s", definedCmd[i].cmdString);
			break;
		}
	}
	return temp;
}

const char * Parser::returnIp(char *str, Source sollEn) {
	testStr[0] = '\0';
	int len = strlen(str);
	int i;
	bool firstFound = false;
	int first;
	int last;
	printf("str %s: %i\n\r", str, sollEn);
	/* first check if its from the Master */
	if ((sollEn == SERVER_CLIENT) && (strstr(str, "Right") != NULL)) {
		for (i = 0; i < len; ++i) {
			if (isdigit(str[i]) && !firstFound) {
				first = i;
				firstFound = true;
			}
		}
		last = i;
		strncpy(testStr, str + first, last);
		testStr[last] = '\0';
	}
	return testStr;
}
