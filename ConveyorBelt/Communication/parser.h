//
//  handler.hpp
//  TestConveyorBelt
//
//  Created by roman passler on 22.04.17.
//  Copyright © 2017 roman passler. All rights reserved.
//

#ifndef handler_hpp
#define handler_hpp

#include <stdio.h>
#include "dataTypes.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

class Parser {
public:
	Parser();
	~Parser();
	Cmd parseCmd(char *str, Source sollEn);
	const char * parseCmd(Cmd cmd);
	void setString();
	static parseCommand definedCmd[];
	const char * returnIp(char *str, Source sollEn);
private:
	char testStr[16];
};

#endif
