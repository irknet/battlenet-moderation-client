#ifndef _LOGHANDLER_H_
#define _LOGHANDLER_H_

#include "Headers.h"

#define LOG_CHAT_WHISPER	1
#define LOG_CONN_ERROR		2
#define LOG_NOTICES			3
#define LOG_COMMANDS		4
#define LOG_STARTUP			5
#define LOG_JOINLEAVE		6

class LogHandler {
private:
	SYSTEMTIME stamp;
	std::ofstream logFile;

public:
	LogHandler() {}
	LogHandler(const LogHandler&);
	LogHandler(std::string filename) {
		logFile.open(filename.c_str(), std::ios::app);
		if(logFile.is_open()) {
			GetLocalTime(&stamp);
			logFile << std::endl << std::endl;
			logFile << "Logging session started at " << stamp.wHour << ":" << stamp.wMinute << ":" << stamp.wSecond << "...";
			logFile << std::endl;
		}
	}
	~LogHandler() {
		if(logFile.is_open()) {
			logFile << "Logging session ended at " << stamp.wHour << ":" << stamp.wMinute << ":" << stamp.wSecond << "...";
			logFile << std::endl;
			logFile.close();
		}
	}

	void openLog(std::string filename);
	bool printToScreen(int type, bool stamped, std::string output, va_list args);
	bool logOutput(int type, bool stamped, std::string output, ...);
};

#endif