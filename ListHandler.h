#ifndef _LISTHANDLER_H_
#define _LISTHANDLER_H_

#include "Headers.h"

#define LIST_BAN	1
#define LIST_CHAN	2

#define WC_BAN		1
#define WC_KICK		2
#define WC_UNBAN	3
#define WC_KICKF	4
#define WC_BANF		5

class ListHandler {
public:
	std::vector<std::string> channel_list;
	std::vector<std::string> banned_list;

	void processWildcard(int listType, int cmdType, std::string wildcardstring, std::string actionMsg);
	void addToList(int listType, std::string username);
	void delFromList(int listType, std::string username);
};

#endif