#ifndef _COMMANDHANDLER_H_
#define _COMMANDHANDLER_H_

#include "Headers.h"

class CommandHandler {
public:
	__int64 startTime;
	int HandleCommand(std::string username, std::string said);
	int wildcmp(const char *wild, const char *string);
	std::string getUptime();
};

#endif