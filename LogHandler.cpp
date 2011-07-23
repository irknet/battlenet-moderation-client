#include "GlobalData.h"
#include "LogHandler.h"

void LogHandler::openLog(std::string filename) {
	logFile.open(filename.c_str(), std::ios::app);
	if(logFile.is_open()) {
		GetLocalTime(&stamp);
		logFile << std::endl << std::endl;
		logFile << "Logging session started at " << stamp.wHour << ":" << stamp.wMinute << ":" << stamp.wSecond << "...";
		logFile << std::endl;
	}
}

bool LogHandler::printToScreen(int type, bool stamped, std::string output, va_list args) {
	switch(type) {
		case LOG_CHAT_WHISPER:
			if(!global->configManip.getlog_chatwhisp())
				return false;
			break;
		case LOG_CONN_ERROR:
			if(!global->configManip.getlog_connerr())
				return false;
			break;
		case LOG_NOTICES:
			if(!global->configManip.getlog_notice())
				return false;
			break;
		case LOG_COMMANDS:
			if(!global->configManip.getlog_cmds())
				return false;
			break;
		case LOG_STARTUP:
			if(!global->configManip.getlog_startmsg())
				return false;
			break;
		case LOG_JOINLEAVE:
			if(!global->configManip.getlog_jnlv())
				return false;
			break;
		default:
			return false;
			break;
	}

	if(stamped) {
		GetLocalTime(&stamp);
		std::cout << "[" << stamp.wHour << ":" << stamp.wMinute << ":" << stamp.wSecond << "] ";
	}

	vprintf(output.c_str(), args);
	va_end(args);

	return true;
}

bool LogHandler::logOutput(int type, bool stamped, std::string output, ...) {
	if(global->configManip.doPrint()) {
		va_list args;
		va_start(args, output);
		printToScreen(type, stamped, output, args);
	}

	if(global->configManip.doLog()) {
		if(logFile.is_open()) {
			switch(type) {
				case LOG_CHAT_WHISPER:
					if(!global->configManip.getlog_chatwhisp())
						return false;
					break;
				case LOG_CONN_ERROR:
					if(!global->configManip.getlog_connerr())
						return false;
						break;
				case LOG_NOTICES:
					if(!global->configManip.getlog_notice())
						return false;
					break;
				case LOG_COMMANDS:
					if(!global->configManip.getlog_cmds())
						return false;
					break;
				case LOG_STARTUP:
					if(!global->configManip.getlog_startmsg())
						return false;
					break;
				case LOG_JOINLEAVE:
					if(!global->configManip.getlog_jnlv())
						return false;
					break;
				default:
					return false;
					break;
			}

			if(stamped) {
				GetLocalTime(&stamp);
				logFile << "[" << stamp.wHour << ":" << stamp.wMinute << ":" << stamp.wSecond << "] ";
			}

			va_list args;
			va_start(args, output);

			char annoying[400];
			vsprintf(annoying, output.c_str(), args);
			va_end(args);
			logFile << annoying;
			return true;
		}
	}
	return false;
}
