#include "GlobalData.h"
#include "BNCSHandler.h"
#include "bncsutil/bncsutil.h"

void BNCSHandler::SendAuthInfo() {
	global->packetManip.Buff.add((int)0);
	global->packetManip.Buff.add("68XI", 4);
	global->packetManip.Buff.add(global->configManip.getClient(), 4);					
	global->packetManip.Buff.add((int)global->configManip.getVerByte());				
	global->packetManip.Buff.add((int)0);				
	global->packetManip.Buff.add((int)0);					
	global->packetManip.Buff.add((int)480);					
	global->packetManip.Buff.add((int)1033);					
	global->packetManip.Buff.add((int)1033);					
	global->packetManip.Buff.add("USA");						
	global->packetManip.Buff.add("United States");
	global->packetManip.Buff.sendpacket(SID_AUTH_INFO);
}

bool BNCSHandler::HandleAuthInfo(DWORD LogonType, DWORD UDPValue, FILETIME MPQFileTime, char *MPQFileName, char *ValueString) {
	switch(LogonType) {
		case 0:
			break;

		case 1:
			global->logManip.logOutput(LOG_CONN_ERROR, true, "The nls is not supported by this bot.\n");
			return false;
			break;

		case 2:
			global->logManip.logOutput(LOG_CONN_ERROR, true, "The nls is not supported by this bot.\n");
			return false;
			break;

		default:
			global->logManip.logOutput(LOG_CONN_ERROR, true, "The suggested logon method is not supported by this client!\n");
			return false;
			break;
	}
	global->logManip.logOutput(LOG_STARTUP, true, "Checking CD key and file versions...\n");

	kd_init();
	unsigned int mpqNum = extractMPQNumber(MPQFileName);
	if(mpqNum < 0)
		return false;

	char *annoying1 = new char[global->configManip.getGamefile1().length() +1];
	char *annoying2 = new char[global->configManip.getGamefile2().length() +1];
	char *annoying3 = new char[global->configManip.getGamefile3().length() +1];
	strcpy(annoying1, global->configManip.getGamefile1().c_str());
	strcpy(annoying2, global->configManip.getGamefile2().c_str());
	strcpy(annoying3, global->configManip.getGamefile3().c_str());
	const char* files[] = { annoying1, annoying2, annoying3 };
	unsigned long Checksum;
	if(!checkRevision(ValueString, files, 3, mpqNum, &Checksum))
		return false;

	delete [] annoying1;
	delete [] annoying2;
	delete [] annoying3;

	DWORD exeVer;
	unsigned int Version;
	char exeInfo[300];
	char *annoying = new char[global->configManip.getGamefile1().length() + 1];
	strcpy(annoying, global->configManip.getGamefile1().c_str());
	exeVer = getExeInfo(annoying, exeInfo, 256, &Version, 0x1);
	delete [] annoying;
	if(!exeVer) {
		global->logManip.logOutput(LOG_CONN_ERROR, true, "Could not gather game file info!\n");
		return false;
	}

	global->packetManip.Buff.add(clientToken);
	global->packetManip.Buff.add(exeVer);
	global->packetManip.Buff.add(Checksum);
	global->packetManip.Buff.add((int)0x01);
	global->packetManip.Buff.add((int)0);

	CDKeyDecoder decoder(global->configManip.getCDKey().c_str(), global->configManip.getCDKey().length());
	if(!decoder.isKeyValid()) {
		global->logManip.logOutput(LOG_CONN_ERROR, true, "The provided CD key is invalid!\n");
		return false;
	}

	unsigned long getProd, getVal;
	getProd = decoder.getProduct();
	getVal = decoder.getVal1();
	global->packetManip.Buff.add((int)strlen(global->configManip.getCDKey().c_str()));
	global->packetManip.Buff.add(getProd);
	global->packetManip.Buff.add(getVal);
	global->packetManip.Buff.add((int)0x00);

	int hashLength = decoder.calculateHash(clientToken, serverToken);
	if(!hashLength)
		return false;

	char KeyHash[64];
	if(!decoder.getHash(KeyHash))
		return false;

	global->packetManip.Buff.add(KeyHash, 5 * 4);
	global->packetManip.Buff.add(exeInfo);
	global->packetManip.Buff.add(global->configManip.getUsername());
	global->packetManip.Buff.sendpacket(SID_AUTH_CHECK);
	return true;
}

bool BNCSHandler::HandleAuthCheck(DWORD Result) {
	switch(Result) {
		case 0x00: {
			global->logManip.logOutput(LOG_STARTUP, true, "CD key and versions accepted!\n");
			SendUDPPingResponse();
			SendGetIconData();
			SendLogonResponse();
			break;
		}

		case 100: {
			global->logManip.logOutput(LOG_CONN_ERROR, true, "Invalid game version!\n");
			return false;
			break;
		}
		case 101: {
			global->logManip.logOutput(LOG_CONN_ERROR, true, "Invalid game version!\n");
			return false;
			break;
		}
		case 202: {
			global->logManip.logOutput(LOG_CONN_ERROR, true, "CD key is banned!\n");
			return false;
			break;
		}
		default: {
			global->logManip.logOutput(LOG_CONN_ERROR, true, "CD key or versions rejected!\n");
			return false;
			break;
		}
	}

	return true;
}

void BNCSHandler::SendUDPPingResponse() {
	global->packetManip.Buff.add((int)'bnet');
	global->packetManip.Buff.sendpacket(SID_UDPPINGRESPONSE);
}

void BNCSHandler::SendGetIconData() {
	global->packetManip.Buff.sendpacket(SID_GETICONDATA);
}

void BNCSHandler::SendLogonResponse() {
	char outBuffer[20];
	char *password = new char[global->configManip.getPassword().length() +1];
	strcpy(password, global->configManip.getPassword().c_str());

	global->logManip.logOutput(LOG_STARTUP, true, "Logging in...\n");
	doubleHashPassword(password, clientToken, serverToken, outBuffer);
	global->packetManip.Buff.add(clientToken);
	global->packetManip.Buff.add(serverToken);
	global->packetManip.Buff.add(outBuffer, 5 * sizeof(unsigned long));
	global->packetManip.Buff.add(global->configManip.getUsername());
	global->packetManip.Buff.sendpacket(SID_LOGONRESPONSE2);

	delete [] password;
}

bool BNCSHandler::HandleLogonResponse(DWORD Result) {
	switch(Result) {
	case 0: {
			global->logManip.logOutput(LOG_STARTUP, true, "Logged in!\n");
			SendEnterChat();
			break;
		}
	case 1:	{
			global->logManip.logOutput(LOG_CONN_ERROR, true, "Failed to login!\n");
			SendCreateAccount();
			break;
		}
	case 2:	{
			global->logManip.logOutput(LOG_CONN_ERROR, true, "Failed to login!\n");
			return false;
			break;
		}
	default: {
			global->logManip.logOutput(LOG_CONN_ERROR, true, "Failed to login - but not sure why ;-)!\n");
			return false;
			break;
		}
	}
	return true;
}

void BNCSHandler::SendCreateAccount() {
	char PassHash[20];

	global->logManip.logOutput(LOG_STARTUP, true, "Attempting to create %s...\n", global->configManip.getUsername().c_str());
	hashPassword(global->configManip.getPassword().c_str(), PassHash);
	global->packetManip.Buff.add(PassHash, 5 * sizeof(unsigned long));
	global->packetManip.Buff.add(global->configManip.getUsername().c_str());
	global->packetManip.Buff.sendpacket(SID_CREATEACCOUNT2);
}

bool BNCSHandler::HandleCreateAccount(DWORD result) {
	switch(result) {
		case 0:
			global->logManip.logOutput(LOG_STARTUP, true, "Account created!\n");
			SendLogonResponse();
			break;
		case 2:
			global->logManip.logOutput(LOG_CONN_ERROR, true, "Username contains invalid characters!\n");
			return false;
			break;
		case 3:
			global->logManip.logOutput(LOG_CONN_ERROR, true, "Username contains banned words!\n");
			return false;
			break;
		case 4:
			global->logManip.logOutput(LOG_CONN_ERROR, true, "Username already exists!\n");
			return false;
			break;
		default:
			global->logManip.logOutput(LOG_CONN_ERROR, true, "Account could not be created!\n");
			return false;
			break;
	}
	return true;
}

void BNCSHandler::SendEnterChat() {
	global->packetManip.Buff.add(global->configManip.getUsername());
	global->packetManip.Buff.add(global->configManip.getClient());
	global->packetManip.Buff.sendpacket(SID_ENTERCHAT);
	global->logManip.logOutput(LOG_STARTUP, true, "Entering chat...\n");

	global->packetManip.Buff.add(global->configManip.getClient(), 4);
	global->packetManip.Buff.sendpacket(SID_GETCHANNELLIST);
	global->logManip.logOutput(LOG_STARTUP, true, "Asking for channel listing...\n");

	global->packetManip.Buff.add((int)2);
	global->packetManip.Buff.add(global->configManip.getHomechannel());
	global->packetManip.Buff.sendpacket(SID_JOINCHANNEL);
	global->logManip.logOutput(LOG_STARTUP, true, "Joining home channel...\n");
}

void BNCSHandler::SendChatCommand(std::string msg) {
    global->packetManip.Buff.add(msg);
	global->packetManip.Buff.sendpacket(SID_CHATCOMMAND);
}

void BNCSHandler::HandleChatEvent(DWORD event, DWORD flags, DWORD ping, char* speaker, char* said) {
	switch(event) {
		case EID_SHOWUSER: {
			std::string username = speaker;
			global->listManip.addToList(LIST_CHAN, username);
			global->logManip.logOutput(LOG_NOTICES, true, "In channel: %s:%02x:%d\n", speaker, flags, ping);
			break;
		}

		case EID_JOIN: {
			std::string username = speaker;
			global->listManip.addToList(LIST_CHAN, username);
			global->logManip.logOutput(LOG_JOINLEAVE, true, "%s:%02x:%d joined the channel\n", speaker, flags, ping);

			if((global->bncsManip.modeProtect == true || global->accessManip.verifyGlobalFlag(username, "B") || global->accessManip.verifyGlobalFlag(username, "Z")) && global->bncsManip.haveGavel == true) {
				if(global->accessManip.verifyGlobalFlag(username, "Z")) {
					std::string buffer = "/ban " + username + " auto ban";
					global->queueManip.AddQueue(buffer, 1);
					break;
				}

				if(global->accessManip.verifyGlobalFlag(username, "B") && !global->accessManip.verifyGlobalFlag(username, "S")) {
					std::string buffer = "/ban " + username + " auto ban";
					global->queueManip.AddQueue(buffer, 1);
					break;
				}

				if(!global->accessManip.verifyGlobalFlag(username, "F") && !global->accessManip.verifyGlobalFlag(username, "S") && global->bncsManip.modeProtect == true) {
					std::string buffer = "/ban " + username + " channel protection";
					global->queueManip.AddQueue(buffer, 1);
					break;
				}
			}
			break;
		}

		case EID_LEAVE: {
			std::string username = speaker;
			global->listManip.delFromList(LIST_CHAN, username);
			global->logManip.logOutput(LOG_JOINLEAVE, true, "%s has left the channel\n", speaker);
			break;
		}

		case EID_WHISPER: {
			std::string person, spoke;
			person = speaker;
			spoke = said;
			global->logManip.logOutput(LOG_CHAT_WHISPER, true, "%s:%02x whispers: %s\n", speaker, flags, said);
			global->commandManip.HandleCommand(person, spoke);
			break;
		}

		case EID_TALK: {
			std::string person, spoke;
			person = speaker;
			spoke = said;
			global->logManip.logOutput(LOG_CHAT_WHISPER, true, "<%s> %s\n", speaker, said);
			if(spoke.substr(0, global->configManip.getTrigger().length()) == global->configManip.getTrigger())
				global->commandManip.HandleCommand(person, spoke.substr(global->configManip.getTrigger().length(), spoke.length()));
			break;
		}

		case EID_BROADCAST: {
			global->logManip.logOutput(LOG_NOTICES, true, "Broadcast: (%s) %s\n", speaker, said);
			break;
		}

		case EID_CHANNEL: {
			haveGavel = false;
			modeProtect = false;
			currentChannel = said;
			global->listManip.channel_list.clear();
			global->listManip.banned_list.clear();
			global->logManip.logOutput(LOG_NOTICES, true, "Joined channel %s\n", said);
			break;
		}

		case EID_USERFLAGS: {
			std::string username = speaker;
			if(flags & 0x02) {
				if(!stricmp(global->bncsManip.realAccount, speaker)) {
					haveGavel = true;
					global->listManip.banned_list.clear();
					global->logManip.logOutput(LOG_NOTICES, true, "You have aquired the gavel\n");
				} else {
					global->logManip.logOutput(LOG_NOTICES, true, "%s has aquired the gavel\n", speaker);
				}
			}
			if((global->bncsManip.modeProtect == true || global->accessManip.verifyGlobalFlag(username, "B") || global->accessManip.verifyGlobalFlag(username, "Z")) && global->bncsManip.haveGavel == true) {
				if(global->accessManip.verifyGlobalFlag(username, "Z")) {
					std::string buffer = "/ban " + username + " auto ban";
					global->queueManip.AddQueue(buffer, 1);
					break;
				}

				if(global->accessManip.verifyGlobalFlag(username, "B") && !global->accessManip.verifyGlobalFlag(username, "S")) {
					std::string buffer = "/ban " + username + " auto ban";
					global->queueManip.AddQueue(buffer, 1);
					break;
				}

				if(!global->accessManip.verifyGlobalFlag(username, "F") && !global->accessManip.verifyGlobalFlag(username, "S") && global->bncsManip.modeProtect == true) {
					std::string buffer = "/ban " + username + " channel protection";
					global->queueManip.AddQueue(buffer, 1);
					break;
				}
			}
			break;
		}

		case EID_WHISPERSENT: {
			global->logManip.logOutput(LOG_CHAT_WHISPER, true, "You whisper %s:%02x: %s\n", speaker, flags, said);
			break;
		}

		case EID_INFO: {
			std::string buffer = said;
			global->logManip.logOutput(LOG_NOTICES, true, "Info: %s\n", said);
			if((global->bncsManip.haveGavel == true) && (buffer.find("was banned by", 0) != std::string::npos)) {
				std::string banned = buffer.substr(0, buffer.find_first_of(" ", 0));
				global->listManip.addToList(LIST_BAN, banned);
			} else if((global->bncsManip.haveGavel == true) && (buffer.find("was unbanned by", 0) != std::string::npos)) {
				std::string unbanned = buffer.substr(0, buffer.find_first_of(" ", 0));
				global->listManip.delFromList(LIST_BAN, unbanned);
			} else if((global->bncsManip.haveGavel == true) && (buffer.find("is your new designated heir", 0) != std::string::npos)) {
				global->bncsManip.designatedUser = buffer.substr(0, buffer.find_first_of(" ", 0));
			}
			break;
		}

		case EID_ERROR: {
			global->logManip.logOutput(LOG_CONN_ERROR, true, "Error: %s\n", said);
			break;
		}

		case EID_EMOTE: {
			global->logManip.logOutput(LOG_CHAT_WHISPER, true, "* %s %s *\n", speaker, said);
			break;
		}

		default: {
			global->logManip.logOutput(LOG_CONN_ERROR, true, "Unknown EID_%02x\n", event);
			break;
		}
	}
}

void BNCSHandler::RejoinChannel() {
	global->packetManip.Buff.sendpacket(SID_LEAVECHAT);
	global->packetManip.Buff.add((int)2);
	global->packetManip.Buff.add(global->bncsManip.currentChannel);
	global->packetManip.Buff.sendpacket(SID_JOINCHANNEL);
}