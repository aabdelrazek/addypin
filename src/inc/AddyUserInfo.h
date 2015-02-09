/*
 * AddyUserInfo.h
 *
 *  Created on: 2015-02-01
 *      Author: aabdelrazek
 */

#ifndef ADDYUSERINFO_H_
#define ADDYUSERINFO_H_
#include <string>

/*! the user info now is just a string with address information, and user name
 * however this might be extended later to contain more information like name, location on a map..etc
 */

class AddyUserInfo {
public:
	AddyUserInfo(std::string addr = "", std::string user = "");
	virtual ~AddyUserInfo();

	std::string& GetAddress();
	std::string& GetUserName();

	std::string Deserialize(std::string& rEntry);
	std::string Serialize(std::string pin);

private:
	std::string mAddress;
	std::string mUserName;
};

#endif /* ADDYUSERINFO_H_ */
