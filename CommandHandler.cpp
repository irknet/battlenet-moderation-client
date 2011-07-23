#include "GlobalData.h"
#include "CommandHandler.h"

int CommandHandler::HandleCommand(std::string username, std::string said) {
	std::string buffer;
	std::transform(said.begin(), said.end(), said.begin(), tolower);
	if(said.substr(0, 4) == "ban " && global->accessManip.verifyGlobalFlag(username, "O") && global->bncsManip.haveGavel == true) {
		std::string banuser, banmsg;
		if(said.find(" ", 4) != std::string::npos) {
			banuser = said.substr(4, said.substr(4, said.length()-4).find_first_of(" "));
			banmsg = said.substr(5 + banuser.length(), said.length()-(4 + banuser.length()));
		} else {
			banuser = said.substr(4, said.length()-4);
			banmsg = "Banned";
		}

		if(banuser.find("*", 0) == std::string::npos && banuser.find("?", 0) == std::string::npos) {
			if(global->accessManip.verifyGlobalFlag(banuser, "S")) {
				banmsg = "/w " + username + " " + banuser + " is safelisted";
				global->queueManip.AddQueue(banmsg, 2);
				return 0;
			} else {
				buffer = "/ban " + banuser + " " + banmsg;
				global->queueManip.AddQueue(buffer, 1);
				return 0;
			}
		} else {
			global->listManip.processWildcard(LIST_CHAN, WC_BAN, banuser, banmsg);
			return 0;
		}
		return 0;
	} else 	if(said.substr(0, 5) == "banf " && global->accessManip.verifyGlobalFlag(username, "O") && global->bncsManip.haveGavel == true) {
		std::string banuser, banmsg;
		if(said.find(" ", 5) != std::string::npos) {
			banuser = said.substr(5, said.substr(5, said.length()-5).find_first_of(" "));
			banmsg = said.substr(6 + banuser.length(), said.length()-(5 + banuser.length()));
		} else {
			banuser = said.substr(5, said.length()-5);
			banmsg = "Banned";
		}

		if(banuser.find("*", 0) == std::string::npos && banuser.find("?", 0) == std::string::npos) {
			if(global->accessManip.verifyGlobalFlag(banuser, "S") || global->accessManip.verifyGlobalFlag(banuser, "F")) {
				banmsg = "/w " + username + " " + banuser + " is safelisted";
				global->queueManip.AddQueue(banmsg, 2);
				return 0;
			} else {
				buffer = "/ban " + banuser + " " + banmsg;
				global->queueManip.AddQueue(buffer, 1);
				return 0;
			}
		} else {
			global->listManip.processWildcard(LIST_CHAN, WC_BANF, banuser, banmsg);
			return 0;
		}
		return 0;
	} else if(said.substr(0, 6) == "unban " && global->accessManip.verifyGlobalFlag(username, "O") && global->bncsManip.haveGavel == true) {
		buffer = said.substr(6, said.length()-6);
		if(buffer.find(" ", 0) != std::string::npos)
			return 0;

		if(buffer.find("*", 0) == std::string::npos && buffer.find("?", 0) == std::string::npos) {
			buffer = "/unban " + buffer;
			global->queueManip.AddQueue(buffer, 1);
		} else {
			global->listManip.processWildcard(LIST_BAN, WC_UNBAN, buffer, "unbanned");
		}
		return 0;
	} else if(said.substr(0, 5) == "kick " && global->accessManip.verifyGlobalFlag(username, "O") && global->bncsManip.haveGavel == true) {
		std::string kickuser, kickmsg;
		if(said.find(" ", 5) != std::string::npos) {
			kickuser = said.substr(5, said.substr(5, said.length()-5).find_first_of(" "));
			kickmsg = said.substr(6 + kickuser.length(), said.length()-(5 + kickuser.length()));
		} else {
			kickuser = said.substr(5, said.length()-5);
			kickmsg = "Kicked";
		}

		if(kickuser.find("*", 0) == std::string::npos && kickuser.find("?", 0) == std::string::npos) {
			if(global->accessManip.verifyGlobalFlag(kickuser, "S")) {
				kickmsg = "/w " + username + " " + kickuser + " is safelisted";
				global->queueManip.AddQueue(kickmsg, 2);
				return 0;
			} else {
				buffer = "/kick " + kickuser + " " + kickmsg;
				global->queueManip.AddQueue(buffer, 1);
				return 0;
			}
		} else {
			global->listManip.processWildcard(LIST_CHAN, WC_KICK, kickuser, kickmsg);
			return 0;
		}
		return 0;
	} else if(said.substr(0, 6) == "kickf " && global->accessManip.verifyGlobalFlag(username, "O") && global->bncsManip.haveGavel == true) {
		std::string kickuser, kickmsg;
		if(said.find(" ", 6) != std::string::npos) {
			kickuser = said.substr(6, said.substr(6, said.length()-6).find_first_of(" "));
			kickmsg = said.substr(7 + kickuser.length(), said.length()-(6 + kickuser.length()));
		} else {
			kickuser = said.substr(6, said.length()-6);
			kickmsg = "Kicked";
		}

		if(kickuser.find("*", 0) == std::string::npos && kickuser.find("?", 0) == std::string::npos) {
			if(global->accessManip.verifyGlobalFlag(kickuser, "S") || global->accessManip.verifyGlobalFlag(kickuser, "F")) {
				kickmsg = "/w " + username + " " + kickuser + " is safelisted";
				global->queueManip.AddQueue(kickmsg, 2);
				return 0;
			} else {
				buffer = "/kick " + kickuser + " " + kickmsg;
				global->queueManip.AddQueue(buffer, 1);
				return 0;
			}
		} else {
			global->listManip.processWildcard(LIST_CHAN, WC_KICKF, kickuser, kickmsg);
			return 0;
		}
		return 0;
	} else if(said.substr(0, 5) == "join " && global->accessManip.verifyGlobalFlag(username, "J")) {
		buffer = "/join " + said.substr(5, said.length());
		global->queueManip.AddQueue(buffer, 2);
		return 0;
	} else if(said == "rejoin" && global->accessManip.verifyGlobalFlag(username, "D")) {
		global->bncsManip.RejoinChannel();
		return 0;
	} else if(said == "quit" && global->accessManip.verifyGlobalFlag(username, "Q")) {
		_endthread();
		return 0;
	} else if(said == "reconnect" && global->accessManip.verifyGlobalFlag(username, "C")) {
		global->packetManip.connected = false;
		return 0;
	} else if(said.substr(0, 5) == "find " && (global->accessManip.verifyGlobalFlag(username, "L") || global->accessManip.verifyGlobalFlag(username, "A"))) {
		if(said == "find ") {
			buffer = "/w " + username + " Syntax: find <entry>";
		} else {

			std::string entry = said.substr(5, said.length()-5);
			std::string entries = global->accessManip.getWildcardMatches(entry);
			std::string globFlags = global->accessManip.getGlobalFlags(entry);

			if(entries == "none") {
				buffer = "/w " + username + " " + entry + " does not exist in the database";
			} else {
				buffer = "/w " + username + " " + entries + " => " + entry + ": " + globFlags;
			}
		}
		global->queueManip.AddQueue(buffer, 2);
		return 0;
	} else if(said.substr(0, 7) == "findex " && (global->accessManip.verifyGlobalFlag(username, "L") || global->accessManip.verifyGlobalFlag(username, "A"))) {
		if(said == "findex ") {
			buffer = "/w " + username + " Syntax: findex <entry>";
		} else {
			std::string entry = said.substr(7, said.length()-7);
			std::string flags = global->accessManip.getExactFlags(entry);

			if(flags == "none") {
				buffer = "/w " + username + " " + entry + " does not exist in the database";
			} else {
				buffer = "/w " + username + " " + entry + " => " + entry + ": " + flags;
			}
		}
		global->queueManip.AddQueue(buffer, 2);
		return 0;
	} else if(said.substr(0, 9) == "findname " && (global->accessManip.verifyGlobalFlag(username, "L") || global->accessManip.verifyGlobalFlag(username, "A"))) {
		if(said == "findname ") {
			buffer = "/w " + username + " Syntax: findname <mask>";
		} else {
			std::string mask = said.substr(9, said.length()-9);
			std::string matches = global->accessManip.getFindname(mask);

			if(matches == "none") {
				buffer = "/w " + username + " No names match " + mask;
			} else {
				buffer = "/w " + username + " " + mask + " => " + matches;
			}
		}
		global->queueManip.AddQueue(buffer, 2);
		return 0;
	} else if(said.substr(0, 9) == "findattr " && (global->accessManip.verifyGlobalFlag(username, "L") || global->accessManip.verifyGlobalFlag(username, "A"))) {
		if(said == "findattr ") {
			buffer = "/w " + username + " Syntax: findattr <flag or flags>";
		} else {
			std::string flags = said.substr(9, said.length()-9);
			std::string matches = global->accessManip.getAttributeMatches(flags);
			
			if(matches == "none")
				buffer = "/w " + username + " No entries have flags " + flags;
			else
				buffer = "/w " + username + " " + matches;
		}

		global->queueManip.AddQueue(buffer, 2);
		return 0;
	} else if((said.substr(0, 4) == "set " || said.substr(0, 4) == "add ") && global->accessManip.verifyGlobalFlag(username, "A")) {
		std::string adduser, dbquery;
		if(said == "set " || said == "add ") {
			buffer = "/w " + username + " Syntax: set <mask> [operation +/-]<flag or flags>";
			global->queueManip.AddQueue(buffer, 2);
			return 0;
		}

		if(said.find(" ", 4) != std::string::npos) {
			adduser = said.substr(4, said.substr(4, said.length()-4).find_first_of(" "));
			dbquery = said.substr(5 + adduser.length(), said.length()-(4 + adduser.length()));
			std::transform(adduser.begin(), adduser.end(), adduser.begin(), tolower);

			if(said.substr(4, said.length()-4) == adduser + " " || said.substr(4, said.length()-4) == adduser + " -" || said.substr(4, said.length()-4) == adduser + " +") {
				adduser = "/w " + username + " Syntax: set <mask> [operation +/-]<flag or flags>";
				global->queueManip.AddQueue(adduser, 2);
				return 0;
			}

			if(!global->accessManip.userExists(adduser)) {
				if(said.substr(4, adduser.length() + 2) == adduser + " -") {
					buffer = "/w " + username + " " + adduser + " does not exist in the database";
					global->queueManip.AddQueue(buffer, 2);
					return 0;
				}
				global->accessManip.addToDatabase(global->configManip.getDatabaseName(), adduser, "X");
				dbquery = "-X+" + dbquery;
				global->accessManip.handleDbQuery(global->configManip.getDatabaseName(), adduser, dbquery);
			} else {
				global->accessManip.handleDbQuery(global->configManip.getDatabaseName(), adduser, dbquery);
				if(global->accessManip.getExactFlags(adduser) == "none") {
					global->accessManip.delUser(global->configManip.getDatabaseName(), adduser);
					buffer = "/w " + username + " " + adduser + " was removed from the database";
					global->queueManip.AddQueue(buffer, 2);
					return 0;
				}
			}

			buffer = "/w " + username + " " + adduser + " => " + global->accessManip.getExactFlags(adduser);
			global->queueManip.AddQueue(buffer, 2);
		} else {
			adduser = "/w " + username + " Syntax: set <mask> [operation +/-]<flag or flags>";
			global->queueManip.AddQueue(adduser, 2);
		}

		if(global->accessManip.verifyGlobalFlag(adduser, "B") == true || global->accessManip.verifyGlobalFlag(adduser, "Z") == true) {
			if(global->accessManip.verifyGlobalFlag(adduser, "S") == true && global->accessManip.verifyGlobalFlag(adduser, "Z") == false)
				return 0;
			buffer = "/ban " + adduser + " auto ban";
			global->queueManip.AddQueue(buffer, 1);
		}
		return 0;
	} else if((said.substr(0, 4) == "del " || said.substr(0, 4) == "rem ") && global->accessManip.verifyGlobalFlag(username, "A")) {
		if(said == "del " || said == "rem ") {
			buffer = "/w " + username + " Syntax: del <entry>";
		} else {
			std::string entry = said.substr(4, said.length()-4);

			if(!global->accessManip.delUser(global->configManip.getDatabaseName(), entry))
				buffer = "/w " + username + " " + entry + " does not exist in the database";
			else
				buffer = "/w " + username + " " + entry + " was removed from the database";
		}
		global->queueManip.AddQueue(buffer, 2);
		return 0;
	} else if(said.substr(0, 4) == "say " && global->accessManip.verifyGlobalFlag(username, "M")) {
		if(said[4] == '/' && !global->accessManip.verifyGlobalFlag(username, "A")) {
			global->queueManip.AddQueue(said.substr(5, said.length()), 2);
			return 0;
		} else {
			global->queueManip.AddQueue(said.substr(4, said.length()), 2);
			return 0;
		}
	} else if((said.substr(0, 4) == "des " || said.substr(0, 10) == "designate ") && global->accessManip.verifyGlobalFlag(username, "D") && global->bncsManip.haveGavel == true) {
		if(said.find(" ", 0) != std::string::npos) {
			buffer = said.substr(said.find_first_of(" ", 0), said.length()-(said.find_first_of(" ", 0)));
			buffer = "/designate " + buffer;
			global->logManip.logOutput(LOG_CHAT_WHISPER, true, "Buffer: %s\n", buffer.c_str());
			global->queueManip.AddQueue(buffer, 1);
			return 0;
		} else {
			buffer = "/designate " + username;
			global->queueManip.AddQueue(buffer, 2);
			return 0;
		}
		return 0;
	} else if(said == "resign" && global->accessManip.verifyGlobalFlag(username, "D") && global->bncsManip.haveGavel == true) {
		buffer = "/resign";
		global->queueManip.AddQueue(buffer, 1);
		return 0;
	} else if(said == "uptime" && global->accessManip.verifyGlobalFlag(username, "O")) {
		buffer = "/w " + username + " " + global->commandManip.getUptime();
        global->queueManip.AddQueue(buffer, 2);
		return 0;
	} else if((said == "ver" || said == "version" || said == "about") && global->accessManip.verifyGlobalFlag(username, "O")) {
		buffer = "/w " + username + " This channel moderation bot by warz was built on 04/28/06 2:40PM";
		global->queueManip.AddQueue(buffer, 2);
		return 0;
	} else if((said == "clearqueue" || said == "cq") && global->accessManip.verifyGlobalFlag(username, "O")) {
		global->queueManip.ClearQueue();
		return 0;
	} else if(said == "trigger" && global->accessManip.verifyGlobalFlag(username, "O")) {
		buffer = "/w " + username + " Bot trigger is " + global->configManip.getTrigger();
		global->queueManip.AddQueue(buffer, 2);
		return 0;
	} else if(said == "checkdesignate" && global->accessManip.verifyGlobalFlag(username, "O") && global->bncsManip.haveGavel == true) {
		if(global->bncsManip.designatedUser.length() > 1)
			buffer = "/w " + username + " " + global->bncsManip.designatedUser + " is currently designated";
		else
			buffer = "/w " + username + " There is currently no designated heir";
		global->queueManip.AddQueue(buffer, 2);
		return 0;
	} else if(said.substr(0, 8) == "protect " && global->accessManip.verifyGlobalFlag(username, "O") && global->bncsManip.haveGavel == true) {
		if(said == "protect ") {
			buffer = "/w " + username + " Syntax: protect <on/off>";
		} else {
			if(said.substr(8, said.length()-8) == "on") {
				if(global->bncsManip.modeProtect) {
					global->bncsManip.modeProtect = true;
					buffer = "/w " + username + " Channel protection was enabled by " + global->bncsManip.protectEnabledBy;
					global->queueManip.AddQueue(buffer, 2);
				} else {
					global->bncsManip.modeProtect = true;
					global->bncsManip.protectEnabledBy = username;
					buffer = "Channel protection enabled by " + global->bncsManip.protectEnabledBy;
					global->queueManip.AddQueue(buffer, 2);
				}
				buffer = "/unsquelch " + global->bncsManip.getRealAccount();
			} else if(said.substr(8, said.length()-8) == "off") {
				if(global->bncsManip.modeProtect) {
					global->bncsManip.protectDisabledBy = username;
					global->bncsManip.modeProtect = false;
					buffer = "Channel protection disabled by " + global->bncsManip.protectDisabledBy;
					global->queueManip.AddQueue(buffer, 2);
				} else {
					global->bncsManip.modeProtect = false;
					buffer = "/w " + username + " Channel protection was disabled by " + global->bncsManip.protectDisabledBy;
					global->queueManip.AddQueue(buffer, 2);
				}
				buffer = "/unsquelch " + global->bncsManip.getRealAccount();
			}
		}
		global->queueManip.AddQueue(buffer, 2);
		return 0;
	}
	return 0;
}

int CommandHandler::wildcmp(const char *wild, const char *string) {
  const char *cp = NULL, *mp = NULL;

  while ((*string) && (*wild != '*')) {
    if ((*wild != *string) && (*wild != '?')) {
      return 0;
    }
    wild++;
    string++;
  }

  while (*string) {
    if (*wild == '*') {
      if (!*++wild) {
        return 1;
      }
      mp = wild;
      cp = string+1;
    } else if ((*wild == *string) || (*wild == '?')) {
      wild++;
      string++;
    } else {
      wild = mp;
      string = cp++;
    }
  }

  while (*wild == '*') {
    wild++;
  }
  return !*wild;
}

std::string CommandHandler::getUptime() {
	char* buf = new char[512];
	std::string uptimes;
	unsigned long sec  = 0;
	unsigned long min  = 0;
	unsigned long hour = 0;
	unsigned long day  = 0;
	LARGE_INTEGER lpPerformanceCount, lpFrequency;
	memset(&lpPerformanceCount, 0, sizeof(lpPerformanceCount));
	memset(&lpFrequency, 0, sizeof(lpFrequency));

		if(QueryPerformanceCounter(&lpPerformanceCount))
			QueryPerformanceFrequency(&lpFrequency);
		__int64 uptime = lpPerformanceCount.QuadPart / lpFrequency.QuadPart;
		sec = static_cast<unsigned long>(uptime);
		if(sec >  60) {
			min = sec / 60;
			sec %= 60;
		}
		if(min >  60) {
			hour = min / 60;
			min %= 60;
		}
		if(hour >  24) {
			day = hour / 24;
			hour %= 24;
		}
		sprintf(buf, "System uptime: %u days, %u hours, %u minutes, %u seconds. ", day, hour, min, sec);
		uptimes = buf;

		memset(&lpPerformanceCount, 0, sizeof(lpPerformanceCount));
		memset(&lpFrequency, 0, sizeof(lpFrequency));
		sec = min = hour = day = 0;

		if(QueryPerformanceCounter(&lpPerformanceCount))
			QueryPerformanceFrequency(&lpFrequency);
		__int64 connectionUptime = lpPerformanceCount.QuadPart / lpFrequency.QuadPart;
		connectionUptime -= global->commandManip.startTime;
		sec = static_cast<unsigned long>(connectionUptime);
		if(sec >  60) {
			min = sec / 60;
			sec %= 60;
		}
		if(min >  60) {
			hour = min / 60;
			min %= 60;
		}
		if(hour >  24) {
			day = hour / 24;
			hour %= 24;
		}
		if(global->commandManip.startTime == (unsigned long)-1)
			sprintf(buf, "There was an error in retrieving the connection uptime!");
		else
			sprintf(buf, "Connection uptime: %d days, %d hours, %d minutes, %d seconds", day, hour, min, sec);
		uptimes = uptimes + buf;
	delete buf;
	return uptimes;
}