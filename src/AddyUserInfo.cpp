/*
 * AddyUserInfo.cpp
 *
 *  Created on: 2015-02-01
 *      Author: aabdelrazek
 */

#include "inc/AddyUserInfo.h"
#include "utils/inc/strtk.hpp"

AddyUserInfo::AddyUserInfo(std::string addr, std::string user): mAddress(addr), mUserName(user) {
}

AddyUserInfo::~AddyUserInfo() {
}

std::string& AddyUserInfo::GetAddress() {
	return mAddress;
}
std::string& AddyUserInfo::GetUserName() {
	return mUserName;
}

/*!
 * decode a 1 string entry from DB file to different fields and return the AddyPin of the entry
 */
std::string AddyUserInfo::Deserialize(std::string& rEntry) {
	std::deque<std::string> strList;
	strtk::parse(rEntry,"||",strList);
	if (strList.size() == 3) {
		mAddress = strList[0];
		mUserName = strList[1];
	}
	return strList[2];
}

/*!
 * encode the user info in 1 string to store to database file and include the given AddyPin in the entry
 */
std::string AddyUserInfo::Serialize(std::string pin) {
	std::string ret = mAddress;
	ret = ret.append("||");
	ret = ret.append(mUserName);
	ret = ret.append("||");
	ret = ret.append(pin);
	return ret;
}
