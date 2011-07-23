#ifndef _ACCESSMANIPULATION_H_
#define _ACCESSMANIPULATION_H_

#include "Headers.h"

class AccessManipulation {
private:
	union UserAttributes {
		struct {
			unsigned A : 1;
			unsigned B : 1;
			unsigned C : 1;
			unsigned D : 1;
			unsigned E : 1;
			unsigned F : 1;
			unsigned G : 1;
			unsigned H : 1;
			unsigned I : 1;
			unsigned J : 1;
			unsigned K : 1;
			unsigned L : 1;
			unsigned M : 1;
			unsigned N : 1;
			unsigned O : 1;
			unsigned P : 1;
			unsigned Q : 1;
			unsigned R : 1;
			unsigned S : 1;
			unsigned T : 1;
			unsigned U : 1;
			unsigned V : 1;
			unsigned W : 1;
			unsigned X : 1;
			unsigned Y : 1;
			unsigned Z : 1; // 26
			unsigned Designated : 1;
			unsigned Operator : 1; // 28
			unsigned Reserved : 4; // 32
		};

		struct {
			unsigned Attributes : 26; // 26
			unsigned Information : 6; // 32
		};

		inline void Set(unsigned char cAttribute) {
			if(cAttribute < 'A' || cAttribute > 'Z')
				return;
			dwFlags |= (1 << (cAttribute - 'A'));
		}

		inline void Set(UserAttributes _Attributes) {
			Attributes |= _Attributes.Attributes;
		}

		inline void Remove(unsigned char cAttribute) {
			if(cAttribute < 'A' || cAttribute > 'Z')
				return;
			dwFlags &= ~(1 << (cAttribute - 'A'));
		}

		inline void Remove(UserAttributes _Attributes) {
			Attributes &= ~_Attributes.Attributes;
		}

		inline bool Check(unsigned char cAttribute) const {
			if(cAttribute < 'A' || cAttribute > 'Z')
				return false;
			return !!(dwFlags & (1 << (cAttribute - 'A')));
		}

		inline bool Check(UserAttributes _Attributes) const {
			return (Attributes & _Attributes.Attributes) == _Attributes.Attributes;
		}

		inline void Replace(UserAttributes _Attributes, bool bReplaceInformation) {
			if(bReplaceInformation)
				dwFlags = _Attributes.dwFlags;
			else
				Attributes = _Attributes.Attributes;
		}

		inline void Reset() {
			dwFlags = 0;
		}

		DWORD dwFlags; // 32
	};

	typedef std::map<std::string, UserAttributes> themap;
	themap accessmap;

public:
	inline bool loadDatabase(std::string filename) {
		std::ifstream database(filename.c_str());

		if(!database.is_open())
			return false;

		std::string username, access;
		while(!database.eof()) {
			database >> username >> access;

			UserAttributes accessmanip;
			accessmanip.Reset();
			for(int x=0; x != access.length(); x++) {
				accessmanip.Set(access[x]);
			}
			std::transform(username.begin(), username.end(), username.begin(), tolower);
			accessmap[username] = accessmanip;
		}
		database.close();
		return true;
	}

	inline void saveDatabase(std::string filename) {
		std::remove(filename.c_str());
		themap::iterator iter = accessmap.begin();

		std::ofstream database(filename.c_str());
		std::string username;
		while(iter != accessmap.end()) {
			username = (*iter).first;
			database << username << " " << getExactFlags(username) << std::endl;
			iter++;
		}
		database.close();
	}

	inline bool addToDatabase(std::string filename, std::string username, std::string access) {
		std::ofstream database(filename.c_str(), std::ios::app);
		std::transform(access.begin(), access.end(), access.begin(), toupper);

		if(!database.is_open())
			return false;

		UserAttributes accessmanip;
		accessmanip.Reset();
		for(int x=0; x != access.length(); x++) {
			accessmanip.Set(access[x]);
		}
		accessmap[username] = accessmanip;

		database << username << " " << access << std::endl;
		database.close();
		saveDatabase(filename.c_str());
		return true;
	}

	inline bool modifyDatabase(std::string filename, std::string username, std::string access) {
		std::transform(username.begin(), username.end(), username.begin(), tolower);
		themap::iterator iter = accessmap.find(username);
		if(iter == accessmap.end())
			return false;

		std::transform(access.begin(), access.end(), access.begin(), toupper);
		UserAttributes accessmanip;

		accessmanip.Reset();
		for(int x=0; x != access.length(); x++) {
			accessmanip.Set(access[x]);
		}
		accessmap[username] = accessmanip;
		saveDatabase(filename.c_str());
		return true;
	}

	inline bool verifyFlag(std::string username, std::string access) {
		std::transform(username.begin(), username.end(), username.begin(), tolower);
		themap::iterator iter = accessmap.find(username);
		std::transform(access.begin(), access.end(), access.begin(), toupper);
		const char flag = (const char)access[0];

		if(iter == accessmap.end())
			return false;
		if(!(*iter).second.Check(flag))
			return false;
		else
			return true;
		return false;
	}

	inline bool removeFlag(std::string filename, std::string username, std::string access) {
		std::transform(username.begin(), username.end(), username.begin(), tolower);
		themap::iterator iter = accessmap.find(username);
		std::transform(access.begin(), access.end(), access.begin(), toupper);

		if(iter == accessmap.end())
			return false;
		const char flag = (const char)access[0];
		if(verifyFlag(username, access)) {
			(*iter).second.Remove(flag);
			if(!verifyFlag(username, access)) {
				saveDatabase(filename.c_str());
				return true;
			} else {
				return false;
			}
		}
		return false;
	}

	inline bool addFlag(std::string filename, std::string username, std::string access) {
		std::transform(username.begin(), username.end(), username.begin(), tolower);
		themap::iterator iter = accessmap.find(username);
		std::transform(access.begin(), access.end(), access.begin(), toupper);

		if(iter == accessmap.end())
			return false;
		const char flag = (const char)access[0];
		if(!verifyFlag(username, access)) {
			(*iter).second.Set(flag);
			if(verifyFlag(username, access)) {
				saveDatabase(filename.c_str());
				return true;
			} else {
				return false;
			}
		}
		return false;
	}

	inline bool delUser(std::string filename, std::string username) {
		std::transform(username.begin(), username.end(), username.begin(), tolower);
		themap::iterator iter = accessmap.find(username);

		if(iter == accessmap.end())
			return false;
		accessmap.erase(username);
		saveDatabase(filename.c_str());
		return true;
	}

	inline bool userExists(std::string username) {
		std::transform(username.begin(), username.end(), username.begin(), tolower);
		themap::iterator iter = accessmap.find(username);
		if(iter == accessmap.end())
			return false;
		return true;
	}

	inline std::string handleDbQuery(std::string filename, std::string username, std::string query) {
		bool add, del;

		std::transform(username.begin(), username.end(), username.begin(), tolower);
		if(!userExists(username))
			return "none";

		add = del = false;
		for(int x=0; x != query.length(); x++) {
			if(query[x] == '+') {
				add = true;
				del = false;
			}
			if(query[x] == '-') {
				add = false;
				del = true;
			}

			if(add) {
				addFlag(filename, username, query.substr(x, 1));
			}
			if(del) {
				removeFlag(filename, username, query.substr(x, 1));
			}
			if(!add) {
				if(!del) {
					if(verifyFlag(username, query.substr(x, 1)))
						removeFlag(filename, username, query.substr(x, 1));
					else
						addFlag(filename, username, query.substr(x, 1));
				}
			}
		}
		return getExactFlags(username);
	}

	inline std::string getExactFlags(std::string username) {
		std::transform(username.begin(), username.end(), username.begin(), tolower);
		themap::iterator iter = accessmap.find(username);
		if(iter == accessmap.end())
			return "none";

		std::string flags, tempflag;
		tempflag = "A";
		int times = 0;
		for(int x=0; x <= 25; x++) {
			if(verifyFlag(username, tempflag)) {
				times++;
				flags = flags + tempflag;
			}
			tempflag[0]++;
		}

		if(times == 0)
			return "none";
		return flags;
	}

	std::string getWildcardMatches(std::string username);
	std::string getFindname(std::string username);
	std::string getAttributeMatches(std::string flags);
	std::string getGlobalFlags(std::string username);
	bool verifyGlobalFlag(std::string, std::string access);
};

#endif