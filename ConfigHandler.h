#ifndef _CONFIGHANDLER_H_
#define _CONFIGHANDLER_H

#include "Headers.h"

class ConfigHandler {
private:
	std::string username, password;
	std::string cdkey, server;
	std::string database_file, client;
	std::string gamefile1, gamefile2, gamefile3;
	std::string homechannel, logfile, trigger;
	bool allowLog, log_chat_whisper, log_conn_error, log_notices;
	bool log_commands, log_startup, log_joinleave, allowPrint;
	bool hideConsole;
	long versionByte;

public:
	inline bool loadConfig(std::string filename) {
		std::ifstream config(filename.c_str());

		if(!config.is_open())
			return false;

		std::string temp;
		trigger = ".";
		client = "RATS";
		allowLog = allowPrint = hideConsole = false;
		log_chat_whisper = log_conn_error = log_notices = log_commands = log_startup = log_joinleave = false;
		while(!config.eof()) {
			getline(config, temp);

			if(temp.substr(0, 2) == "//") {
				// do nothing
			} else {
				if(temp.length() < 2) {
					// do nothing
				}
				if(temp.substr(0, 9) == "username=")
					username = temp.substr(9, temp.length());
				if(temp.substr(0, 9) == "password=")
					password = temp.substr(9, temp.length());
				if(temp.substr(0, 7) == "client=")
					client = temp.substr(7, temp.length());
				if(temp.substr(0, 6) == "cdkey=")
					cdkey = temp.substr(6, temp.length());
				if(temp.substr(0, 7) == "server=")
					server = temp.substr(7, temp.length());
				if(temp.substr(0, 9) == "database=")
					database_file = temp.substr(9, temp.length());
				if(temp.substr(0, 6) == "file1=")
					gamefile1 = temp.substr(6, temp.length());
				if(temp.substr(0, 6) == "file2=")
					gamefile2 = temp.substr(6, temp.length());
				if(temp.substr(0, 6) == "file3=")
					gamefile3 = temp.substr(6, temp.length());
				if(temp.substr(0, 9) == "homechan=")
					homechannel = temp.substr(9, temp.length());
				if(temp.substr(0, 8) == "trigger=")
					trigger = temp.substr(8, temp.length());
				if(temp.substr(0, 8) == "version=") {
					char *pEnd;
					versionByte = strtol(temp.substr(8, temp.length()).c_str(), &pEnd, 0);
				}
				if(temp == "log")
					allowLog = true;
				if(temp == "print")
					allowPrint = true;
				if(temp == "hideconsole")
					hideConsole = true;
				if(temp.substr(0, 8) == "logfile=")
					logfile = temp.substr(8, temp.length());
				if(allowLog || allowPrint) {
					if(temp.substr(0, 15) == "logsensitivity=") {
						for(int i = 15; i != temp.length(); i++) {
							if(temp[i] == 'C' || temp[i] == 'c')
								log_chat_whisper = true;
							if(temp[i] == 'E' || temp[i] == 'e')
								log_conn_error = true;
							if(temp[i] == 'J' || temp[i] == 'j')
								log_joinleave = true;
							if(temp[i] == 'N' || temp[i] == 'n')
								log_notices = true;
							if(temp[i] == 'R' || temp[i] == 'r')
								log_commands = true;
							if(temp[i] == 'S' || temp[i] == 's')
								log_startup = true;
						}
					}
				}
			}
		}
		config.close();
		return true;
	}

	inline std::string getDatabaseName() { return database_file; }
	inline std::string getUsername() { return username; }
	inline std::string getPassword() { return password; }
	inline std::string getServer() { return server; }
	inline std::string getCDKey() { return cdkey; }
	inline std::string getClient() { return client; }
	inline std::string getGamefile1() { return gamefile1; }
	inline std::string getGamefile2() { return gamefile2; }
	inline std::string getGamefile3() { return gamefile3; }
	inline std::string getHomechannel() { return homechannel; }
	inline std::string getLogfile() { return logfile; }
	inline std::string getTrigger() { return trigger; }

	inline bool doLog() { return allowLog; }
	inline bool doPrint() { return allowPrint; }
	inline bool doHide() { return hideConsole; }
	inline bool getlog_chatwhisp() { return log_chat_whisper; }
	inline bool getlog_connerr() { return log_conn_error; }
	inline bool getlog_notice() { return log_notices; }
	inline bool getlog_cmds() { return log_commands; }
	inline bool getlog_startmsg() { return log_startup; }
	inline bool getlog_jnlv() { return log_joinleave; }

	inline long getVerByte() { return versionByte; }
};

#endif