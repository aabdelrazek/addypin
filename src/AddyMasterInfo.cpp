/*
 * AddyMasterInfo.cpp
 *
 *  Created on: 2015-02-14
 *      Author: aabdelrazek
 */

#include "inc/AddyMasterInfo.h"
#include "utils/inc/strtk.hpp"
#include <Wt/WApplication>

const std::string AddyMasterInfo::kSeparator = "**";

AddyMasterInfo::AddyMasterInfo(std::string email, std::string mpin, unsigned int maxEntries):
		mMasterPin(mpin),
		mEmail(email),
		mMaxEntries(maxEntries) {
}

AddyMasterInfo::~AddyMasterInfo() {
}

const std::string& AddyMasterInfo::GetMasterPin() {
	return mMasterPin;
}

const std::string& AddyMasterInfo::GetEmail() {
	return mEmail;
}

const unsigned int AddyMasterInfo::GetNumEntries() {
	return mUserInfoEntries.size();
}

AddyUserInfo* AddyMasterInfo::GetEntry(unsigned int id) {
	if (id < mUserInfoEntries.size()) {
		return mUserInfoEntries[id];
	}
	return NULL;
}

/*!
 * decode a 1 string entry from DB file to different fields and return the AddyPin of the entry
 */
void AddyMasterInfo::Deserialize(const std::string& rEntry) {
	std::deque<std::string> strList;
	strtk::parse(rEntry, kSeparator, strList);
	mMasterPin = strList[0];
	mEmail = strList[1];
	unsigned int listSize;
	std::istringstream convert(strList[2]);
	if (!(convert >> listSize)) {
		Wt::log("error")<<"AddyMasterInfo::Deserialize erorrrrrrrrrrrrr";
		listSize = 0;
	}
	for (unsigned int i = 0; i< listSize; i++) {
		AddyUserInfo* pNew = new AddyUserInfo();
		pNew->Deserialize(strList[3+i]);
		mUserInfoEntries.push_back(pNew);
	}
}


/*!
 * encode the user info in 1 string to store to database file and include the given AddyPin in the entry
 */
void AddyMasterInfo::Serialize(std::string& rEntry) {
	std::string& ret = rEntry;
	ret = ret.append(mMasterPin);
	ret = ret.append(kSeparator);
	ret = ret.append(mEmail);
	ret = ret.append(kSeparator);
	std::ostringstream oss;
	oss<< mUserInfoEntries.size();
	ret = ret.append(oss.str());
	ret = ret.append(kSeparator);
	for (std::vector<AddyUserInfo*>::iterator it = mUserInfoEntries.begin(); it != mUserInfoEntries.end(); it++) {
		(*it)->Serialize(ret);
		ret = ret.append(kSeparator);
	}
}

bool AddyMasterInfo::AddNewEntry(AddyUserInfo* pEntry) {
	bool ret = false;
	if (mUserInfoEntries.size() < mMaxEntries) {
		mUserInfoEntries.push_back(pEntry);
		ret = true;
	}

	return ret;
}

void AddyMasterInfo::DeleteAllEntries() {
	mUserInfoEntries.clear();
}
