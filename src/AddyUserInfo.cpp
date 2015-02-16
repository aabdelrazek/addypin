/*
 * AddyUserInfo.cpp
 *
 *  Created on: 2015-02-01
 *      Author: aabdelrazek
 */

#include "inc/AddyUserInfo.h"
#include "utils/inc/strtk.hpp"
#include <Wt/WApplication>

const std::string AddyUserInfo::kSeparator = "||";

AddyUserInfo::AddyUserInfo(std::string addr, std::string pin): mAddress(addr) {
	mPin.SetPin(pin);
}

AddyUserInfo::~AddyUserInfo() {
}

const std::string& AddyUserInfo::GetAddress() {
	return mAddress;
}

const std::string& AddyUserInfo::GetPin() {
	return mPin.GetPin();
}

/*!
 * decode a 1 string entry from DB file to different fields of the entry
 */
void AddyUserInfo::Deserialize(const std::string& rEntry) {
	std::deque<std::string> strList;
	strtk::parse(rEntry, kSeparator, strList);
	if (strList.size() == 2) {
		mAddress = strList[0];
		mPin.SetPin(strList[1]);
	} else {
		Wt::log("error")<<"AddyUserInfo::Deserialize erorrrrrrrrrrrr";
	}
}

/*!
 * encode the user info in 1 string to store to database file and include the given AddyPin in the entry
 */
void AddyUserInfo::Serialize(std::string& rEntry) {
	std::string& ret = rEntry;
	ret = ret.append(mAddress);
	ret = ret.append(kSeparator);
	ret = ret.append(mPin.GetPin());
}
