#ifndef _BNCSHANDLER_H_
#define _BNCSHANDLER_H_

#include "Headers.h"

#define SID_ENTERCHAT			0x0A
#define SID_GETCHANNELLIST		0x0B
#define SID_JOINCHANNEL			0x0C
#define SID_CHATCOMMAND			0x0E
#define SID_CHATEVENT			0x0F
#define SID_LEAVECHAT			0x10
#define SID_UDPPINGRESPONSE		0x14
#define SID_PING				0x25
#define SID_GETICONDATA			0x2D
#define SID_LOGONRESPONSE2		0x3A
#define SID_CREATEACCOUNT2		0x3D
#define SID_AUTH_INFO			0x50
#define SID_AUTH_CHECK			0x51

#define EID_SHOWUSER			0x01  
#define EID_JOIN				0x02  
#define EID_LEAVE				0x03  
#define EID_WHISPER				0x04  
#define EID_TALK				0x05  
#define EID_BROADCAST			0x06  
#define EID_CHANNEL				0x07  
#define EID_USERFLAGS			0x09  
#define EID_WHISPERSENT			0x0A  
#define EID_CHANNELFULL			0x0D  
#define EID_CHANNELDOESNOTEXIST	0x0E  
#define EID_CHANNELRESTRICTED	0x0F  
#define EID_INFO				0x12  
#define EID_ERROR				0x13  
#define EID_EMOTE				0x17  

class BNCSHandler {
private:
	DWORD serverToken;
	DWORD clientToken;

public:
	std::string currentChannel, designatedUser, protectEnabledBy, protectDisabledBy;
	char realAccount[64];
	bool haveGavel, modeProtect, recvError;
	BNCSHandler() { clientToken = GetTickCount(); }

	inline void setServerToken(DWORD token) { serverToken = token; }
	inline DWORD getClientToken() { return clientToken; }
	inline DWORD getServerToken() { return serverToken; }
	inline std::string getRealAccount() { std::string raccount = realAccount; return raccount; }

	void SendAuthInfo();
	bool HandleAuthInfo(DWORD LogonType, DWORD UDPValue, FILETIME MPQFileTime, char *MPQFileName, char *ValueString);
	bool HandleAuthCheck(DWORD Result);
	void SendUDPPingResponse();
	void SendGetIconData();
	void SendLogonResponse();
	bool HandleLogonResponse(DWORD Result);
	void SendCreateAccount();
	bool HandleCreateAccount(DWORD result);
	void SendEnterChat();
	void SendChatCommand(std::string msg);
	void HandleChatEvent(DWORD event, DWORD flags, DWORD ping, char* speaker, char* said);
	void RejoinChannel();
};

#endif