#ifndef _PACKETHANDLER_H_
#define _PACKETHANDLER_H_

#include "Headers.h"

class DataBuffer {
private:
	char buffer[8192];
	int packetpos;

public:
	SOCKET sock;

	DataBuffer() {
		packetpos = 4;
	}

	inline int sendpacket(int id) {
		int bytesSent = 0;
		buffer[0] = (char)0xff;
		buffer[1] = (char)id;
		*(WORD*)(buffer + 2) = (WORD)packetpos;
		bytesSent = send(sock, buffer, packetpos, 0);
		if(bytesSent == SOCKET_ERROR) {
			return SOCKET_ERROR;
		}
		packetpos = 4;
		return bytesSent;
	}

	inline void add(char *p) {
		strcpy(buffer + packetpos, p);
		packetpos += strlen(p) + 1;
	}

	inline void add(std::string p) {
		strcpy(buffer + packetpos, p.c_str());
		packetpos += strlen(p.c_str()) + 1;
	}

	inline void add(void *p, int len) {
		memcpy(buffer + packetpos, p, len);
		packetpos += len;
	}

	inline void add(std::string p, int len) {
		memcpy(buffer + packetpos, p.c_str(), len);
		packetpos += len;
	}

	inline void add(int v) {
		memcpy(buffer + packetpos, &v, 4);
		packetpos += 4;
	}
};

class DataDebuffer {
private:
	unsigned char *recpack;
	int recpacklen;
	int recpackpos;

public:
	inline void packet(unsigned char *p, int len) {
		recpack = p;
		recpacklen = len;
		recpackpos = 4;
	}

	inline DWORD get(void) {
		if(recpacklen - recpackpos >= 4) {
			recpackpos += 4;
			return *(DWORD*)(recpack + recpackpos - 4);
		} else { 
			return 0; 
		}
	}

	inline char *get(char *p) {
		if(memchr(recpack + recpackpos, 0, recpacklen - recpackpos)) {
			strcpy(p, (char*)recpack + recpackpos);
			recpackpos += strlen(p) + 1;
		} else {
			memcpy(p, recpack + recpackpos, recpacklen - recpackpos);
			p[recpacklen - recpackpos] = 0;
			recpackpos = recpacklen;
		}
		return p;
	}
};

class PacketHandler {
public:
	bool connected;
	
	DataBuffer Buff;
	DataDebuffer DeBuff;
};

#endif