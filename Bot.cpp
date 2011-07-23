#include "GlobalData.h"

unsigned __stdcall botMain(void *threadname);
void CALLBACK QueueThread(void);

int main(int argc, char *argv[]) {
	if(!global->configManip.loadConfig("bot.cfg"))
		return 0;

	if(global->configManip.doHide())
		FreeConsole();

	if(global->configManip.doLog())
		global->logManip.openLog(global->configManip.getLogfile());

	global->logManip.logOutput(LOG_STARTUP, true, "Populating user map...");
	if(!global->accessManip.loadDatabase(global->configManip.getDatabaseName())) {
		global->logManip.logOutput(LOG_CONN_ERROR, true, "FAILED TO POPULATE USER MAP!\n");
		return 0;
	}
	global->logManip.logOutput(LOG_STARTUP, false, "OK\n");
	global->packetManip.connected = false;
	global->bncsManip.recvError = false;

	char annoying[25];
	unsigned threadID;
	LARGE_INTEGER Counter, Frequency;

	do {
		QueryPerformanceCounter(&Counter);
		QueryPerformanceFrequency(&Frequency);
		global->commandManip.startTime = Counter.QuadPart / Frequency.QuadPart;

		strncpy(annoying, global->configManip.getUsername().c_str(), 25);
		
		HANDLE hThread = (HANDLE)_beginthreadex( NULL, 0, &botMain, annoying, 0, &threadID );
		WaitForSingleObject(hThread, INFINITE);
	} while(global->packetManip.connected == false && global->bncsManip.recvError == false);

	return 0;
}

unsigned __stdcall botMain(void *threadname) {
	SOCKADDR_IN serverInfo;
	struct hostent *hstEnt;
	WSADATA wsa;
	WSAEVENT event = NULL;

	if(WSAStartup(2, &wsa) || wsa.wVersion != 2) {
		return 0;
	}

	global->bncsManip.modeProtect = false;
	global->packetManip.connected = false;
	global->queueManip.queueEvent = 0;
	global->logManip.logOutput(LOG_STARTUP, true, "Current thread: %s\n", threadname);

	do {
		global->packetManip.Buff.sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(global->packetManip.Buff.sock == INVALID_SOCKET) {
			global->bncsManip.recvError = true;
			return 0;
		}

		char *p = (char *)global->configManip.getServer().c_str();
		memset(&serverInfo, '\0', sizeof(serverInfo));
		serverInfo.sin_family = AF_INET;
		serverInfo.sin_port = htons(6112);
		while (*p && (isdigit(static_cast<unsigned char>(*p)) || (*p == '.')))
			p++;
		if(*p){
			hstEnt = gethostbyname(global->configManip.getServer().c_str());
			if(hstEnt == 0)
				return false;
			memcpy(&serverInfo.sin_addr, hstEnt->h_addr, hstEnt->h_length);
		}
		else
			serverInfo.sin_addr.s_addr = inet_addr(global->configManip.getServer().c_str());
	
		global->logManip.logOutput(LOG_STARTUP, true, "Connecting...\n");
		if(connect(global->packetManip.Buff.sock, (struct sockaddr *)&serverInfo, sizeof(serverInfo)) == SOCKET_ERROR) {
			global->logManip.logOutput(LOG_CONN_ERROR, true, "Could not connect!\n");
			global->bncsManip.recvError = true;
			return 0;
		}
		global->packetManip.connected = true;
	} while(global->packetManip.connected == false);

	global->queueManip.queueThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)QueueThread, NULL, NULL, NULL);
	global->logManip.logOutput(LOG_STARTUP, true, "Queue thread started...\n");
	global->logManip.logOutput(LOG_STARTUP, true, "Connected!\n");
	event = CreateEvent(0, 0, 0, 0);
	if(!event)
		return 0;

	send(global->packetManip.Buff.sock, "\x1", 1, 0);
	global->bncsManip.SendAuthInfo();
	WSAEventSelect(global->packetManip.Buff.sock, event, FD_READ | FD_CLOSE);

	int recvlen, waitresult, buflen;
	unsigned char buffer[4049];
	buflen = 0;
	while(global->packetManip.connected) {
		waitresult = WaitForSingleObject(event, INFINITE);
		switch(waitresult) {
			case WAIT_ABANDONED:
				break;
			case WAIT_OBJECT_0:
				break;
			case WAIT_TIMEOUT:
				break;
			case WAIT_FAILED:
				break;
			default:
				break;
		}

		recvlen = recv(global->packetManip.Buff.sock, (char*)buffer + buflen, 1024, 0);
		if(recvlen == 0 || recvlen == SOCKET_ERROR || recvlen == WSAETIMEDOUT) {
			if(recvlen == SOCKET_ERROR)
				global->logManip.logOutput(LOG_CONN_ERROR, true, "Socket error!\n");
			global->logManip.logOutput(LOG_CONN_ERROR, true, "Connection closed!\n");
			global->packetManip.connected = false;
			break;
		}

		buflen += recvlen;
		while(buflen >= 4) {
			short packetlen = *(short*)(buffer + 2);

			if(buffer[0] != 0xff) {
				global->logManip.logOutput(LOG_CONN_ERROR, true, "Unknown protocol ID!\n");
				global->bncsManip.recvError = true;
				global->packetManip.connected = false;
				break;
			}
			if(buflen < packetlen)
				break;

			global->packetManip.DeBuff.packet(buffer, packetlen);
			switch(buffer[1]) {
				case SID_PING: {
					DWORD ping = global->packetManip.DeBuff.get();
					global->packetManip.Buff.add(ping);
					global->packetManip.Buff.sendpacket(SID_PING);
					break;
				}

				case SID_CHATEVENT: {
					char speaker[64];
					char said[256];
					DWORD event = global->packetManip.DeBuff.get();
					DWORD flags = global->packetManip.DeBuff.get();
					DWORD ping = global->packetManip.DeBuff.get();
					DWORD ip = global->packetManip.DeBuff.get();
					DWORD acctnum = global->packetManip.DeBuff.get();
					DWORD regauth = global->packetManip.DeBuff.get();
					global->packetManip.DeBuff.get(speaker);
					global->packetManip.DeBuff.get(said);
				
					global->bncsManip.HandleChatEvent(event, flags, ping, speaker, said);
					break;
				}

				case SID_ENTERCHAT: {
					global->packetManip.DeBuff.get(global->bncsManip.realAccount);
					global->logManip.logOutput(LOG_NOTICES, true, "Entered chat as %s\n", global->bncsManip.realAccount);
					break;
				}

				case SID_GETCHANNELLIST:
					break;

				case SID_GETICONDATA:
					break;

				case SID_UDPPINGRESPONSE:
					break;

				case SID_LOGONRESPONSE2: {
					DWORD result = global->packetManip.DeBuff.get();
					if(!global->bncsManip.HandleLogonResponse(result)) {
						global->bncsManip.recvError = true;
						global->packetManip.connected = false;
					}
					break;
				}

				case SID_CREATEACCOUNT2: {
					DWORD result = global->packetManip.DeBuff.get();
					if(!global->bncsManip.HandleCreateAccount(result)) {
						global->bncsManip.recvError = true;
						global->packetManip.connected = false;
					}
					break;
				}

				case SID_AUTH_INFO: {
					DWORD token;
					char MPQFileName[24];
					char Formula[256];
					FILETIME MPQFileTime;

					DWORD logonType = global->packetManip.DeBuff.get();
					token = global->packetManip.DeBuff.get();
					DWORD UDPValue = global->packetManip.DeBuff.get();
					MPQFileTime.dwLowDateTime = global->packetManip.DeBuff.get();
					MPQFileTime.dwHighDateTime = global->packetManip.DeBuff.get();
					global->packetManip.DeBuff.get(MPQFileName);
                    global->packetManip.DeBuff.get(Formula);
					
					global->bncsManip.setServerToken(token);
					if(!global->bncsManip.HandleAuthInfo(logonType, UDPValue, MPQFileTime, MPQFileName, Formula)) {
						global->bncsManip.recvError = true;
						global->packetManip.connected = false;
					}
					break;
				}

				case SID_AUTH_CHECK: {
					DWORD result = global->packetManip.DeBuff.get();

					if(result == 0x201) {
						char inUseBy[64];

						global->packetManip.DeBuff.get(inUseBy);
						global->logManip.logOutput(LOG_CONN_ERROR, true, "The CD key is in use by %s!\n", inUseBy);
						global->bncsManip.recvError = true;
						global->packetManip.connected = false;
					} else {
						if(!global->bncsManip.HandleAuthCheck(result)) {
							global->bncsManip.recvError = true;
							global->packetManip.connected = false;
						}
					}
					break;
				}

				default:
					break;
			}

			MoveMemory(buffer, buffer + packetlen, buflen - packetlen);
			buflen -= packetlen;
		}

		if(buflen > 3024) {
			MoveMemory(buffer, buffer + buflen-1024, 1025);
			buflen = 1024;
		}
	}

	global->logManip.logOutput(LOG_STARTUP, true, "Releasing winsock DLL...\n");
	if(event)
		WSACloseEvent(event);

	WSACleanup();
	_endthreadex(0);
	return 0;
}

void CALLBACK QueueThread(void) {
	while(global->packetManip.connected) {
		if(!global->queueManip.queue.empty()) {
			if(GetTickCount() > global->queueManip.sendQueuedMessage) {
				global->bncsManip.SendChatCommand(global->queueManip.queue.top().Text);
				global->queueManip.sendQueuedMessage = GetTickCount() + global->queueManip.CalculateQueueDelayEx((DWORD)global->queueManip.queue.top().Text.length());
				global->queueManip.queue.pop();
				DWORD wait = global->queueManip.sendQueuedMessage - GetTickCount();
				WaitForSingleObject(global->queueManip.queueEvent, wait);
			}
		} else if(global->queueManip.queue.empty()) {
			SuspendThread(global->queueManip.queueThread);
		}
		Sleep(0);
	}
}