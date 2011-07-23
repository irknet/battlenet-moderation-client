#include "GlobalData.h"
#include "AccessManipulation.h"

std::string AccessManipulation::getWildcardMatches(std::string username) {
	std::transform(username.begin(), username.end(), username.begin(), tolower);
	themap::iterator iter = accessmap.begin();

	int times = 0;
	std::string matches;
	for(int x = 0; x < accessmap.size(); x++) {
		if(global->commandManip.wildcmp((*iter).first.c_str(), username.c_str()) || username == (*iter).first) {
			times++;
			if(times == 1)
				matches = (*iter).first;
			else
				matches += " " + (*iter).first;
		}
		iter++;
	}

	if(times == 0)
		return "none";
	else
		return matches;
	return "none";
}

std::string AccessManipulation::getFindname(std::string username) {
	std::transform(username.begin(), username.end(), username.begin(), tolower);
	themap::iterator iter = accessmap.begin();

	int times = 0;
	std::string matches;
	for(int x = 0; x < accessmap.size(); x++) {
		if(global->commandManip.wildcmp(username.c_str(), (*iter).first.c_str())) {
			times++;
			if(times == 1)
				matches = (*iter).first;
			else
				matches += ", " + (*iter).first;
		}
		iter++;
	}

	if(times == 0)
		return "none";
	else
		return matches;
	return "none";
}

std::string AccessManipulation::getAttributeMatches(std::string flags) {
	std::transform(flags.begin(), flags.end(), flags.begin(), toupper);
	themap::iterator iter = accessmap.begin();

	int times = 0;
	std::string username, matches, usersFlags;

	for(int x = 0; x < accessmap.size(); x++) {
		bool hasAll = true;

		username = (*iter).first;
		for(int y = 0; y <= flags.length() - 1; y++) {
			if(!verifyFlag(username, flags.substr(y, 1))) {
				hasAll = false;
				break;
			}
		}

		if(hasAll) {
			times++;
			usersFlags = getExactFlags(username);
			if(times == 1) {
				matches = username + ": " + usersFlags;
			} else {
				matches += ", " + username + ": " + usersFlags;
			}
		}

		iter++;
	}

	if(times == 0)
		return "none";
	else
		return matches;
	return "none";
}

std::string AccessManipulation::getGlobalFlags(std::string username) {
	std::transform(username.begin(), username.end(), username.begin(), tolower);
	themap::iterator iter = accessmap.begin();

	int times = 0;
	std::string usersflags, totalflags;
	for(int x = 0; x < accessmap.size(); x++) {
		if(global->commandManip.wildcmp((*iter).first.c_str(), username.c_str()) || username == (*iter).first) {
			usersflags = getExactFlags((*iter).first);

			for(int y = 0; y < usersflags.length(); y++) {
				if(totalflags.find(usersflags.substr(y, 1), 0) == std::string::npos) {
					totalflags += usersflags.substr(y, 1);
				}
			}
			times++;
		}
		iter++;
	}

	if(times == 0)
		return "none";
	else
		return totalflags;
    return "none";
}

bool AccessManipulation::verifyGlobalFlag(std::string username, std::string access) {
	std::transform(access.begin(), access.end(), access.begin(), toupper);
	if(getGlobalFlags(username).find(access.substr(0, 1), 0) == std::string::npos)
		return false;
	else
		return true;
	return false;
}