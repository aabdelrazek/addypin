/*
 * AddyUserInfo.h
 *
 *  Created on: 2015-02-01
 *      Author: aabdelrazek
 */

#ifndef ADDYUSERINFO_H_
#define ADDYUSERINFO_H_
#include <string>
#include <AddyPin.h>
/*! the user info now is just a string with address information, and user name
 * however this might be extended later to contain more information like name, location on a map..etc
 */

class AddyUserInfo {
public:
	AddyUserInfo(std::string addr = "", std::string pin = "");
	virtual ~AddyUserInfo();

	const std::string& GetAddress();
	const std::string& GetPin();

	void Deserialize(const std::string& rEntry);
	void Serialize(std::string& rEntry);

private:
	std::string mAddress;
	AddyPin mPin;
	static const std::string kSeparator;
};

#endif /* ADDYUSERINFO_H_ */
