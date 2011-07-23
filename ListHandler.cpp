#include "GlobalData.h"
#include "ListHandler.h"

void ListHandler::addToList(int listType, std::string username) {
	std::transform(username.begin(), username.end(), username.begin(), tolower);

	switch(listType) {
		case LIST_CHAN: {
			global->listManip.channel_list.insert(global->listManip.channel_list.end(), 1, username);
			break;
		}

		case LIST_BAN: {
			global->listManip.banned_list.insert(global->listManip.banned_list.end(), 1, username);
			break;
		}
	}
}

void ListHandler::delFromList(int listType, std::string username) {
	std::transform(username.begin(), username.end(), username.begin(), tolower);
	std::vector<std::string>::iterator iter;
	
	switch(listType) {
		case LIST_CHAN: {
			iter = global->listManip.channel_list.begin();
			for(int x = 0; x <= global->listManip.channel_list.size(); x++) {
				if((*iter) == username) {
					global->listManip.channel_list.erase(iter);
					break;
				}
				iter++;
			}  
			break;
		}

		case LIST_BAN: {
			iter = global->listManip.banned_list.begin();
			for(int x = 0; x <= global->listManip.banned_list.size(); x++) {
				if((*iter) == username) {
					global->listManip.banned_list.erase(iter);
					break;
				}
				iter++;
			}  
			break;
		}
	}
}

void ListHandler::processWildcard(int listType, int cmdType, std::string wildcardstring, std::string actionMsg) {
	std::transform(wildcardstring.begin(), wildcardstring.end(), wildcardstring.begin(), tolower);
	std::vector<std::string>::iterator iter;
	
	switch(listType) {
		case LIST_CHAN: {
			for(iter = global->listManip.channel_list.begin(); iter != global->listManip.channel_list.end(); iter++) {
				if(global->commandManip.wildcmp(wildcardstring.c_str(), (*iter).c_str())) {
					if(!global->accessManip.verifyFlag((*iter), "S")) {
						std::string buffer;
						switch(cmdType) {
							case WC_BAN:
								buffer = "/ban " + (*iter) + " " + actionMsg;
								break;
							case WC_KICK:
								buffer = "/kick " + (*iter) + " " + actionMsg;
								break;
							case WC_BANF: {
								if(!global->accessManip.verifyFlag((*iter), "F"))
									buffer = "/ban " + (*iter) + " " + actionMsg;
								break;
							}
							case WC_KICKF: {
								if(!global->accessManip.verifyFlag((*iter), "F"))
									buffer = "/kick " + (*iter) + " " + actionMsg;
								break;
							}
						}
						global->queueManip.AddQueue(buffer, 1);
					}
				}
			}  
			break;
		}
						
		case LIST_BAN: {
			for(iter = global->listManip.banned_list.begin(); iter != global->listManip.banned_list.end(); iter++) {
				global->logManip.logOutput(LOG_CHAT_WHISPER, true, "Banned user: %s\n", (*iter).c_str());
				if(global->commandManip.wildcmp(wildcardstring.c_str(), (*iter).c_str())) {
					std::string buffer;
					switch(cmdType) {
						case WC_UNBAN:
							buffer = "/unban " + (*iter);
							break;
					}
					global->queueManip.AddQueue(buffer, 1);
				}
			}  
			break;
		}
	}
}