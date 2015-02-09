/*
 * AddyDB.cpp
 *
 *  Created on: 2015-02-01
 *      Author: aabdelrazek
 */

#include "inc/AddyDB.h"
#include <string>
#include <stdio.h>
#include "utils/inc/CException.hpp"

#include <stdio.h>

const std::string AddyDB::kDbPath = "/home/aabdelrazek/addypin/addypin/";
const std::string AddyDB::kDbFile = "addypin_data.hdf";

AddyDB::AddyDB() :
		mHdfFileAdapter(kDbPath+kDbFile),
		mHdfManager(&mHdfFileAdapter) {
	// intentionally left blank
	try {
		LoadMap();
	} catch (CException& e) {
		printf("no data to load!!!!!\n");
	}
}

AddyDB::~AddyDB() {
	try {
		SaveMap();
	}catch (CException& e) {
		printf("Failed to save!!!!!\n");
	}
}

AddyPin AddyDB::Add(AddyUserInfo& user) {

	AddyPin addyPin;
	mAddyPinAllocator.AllocatePin(addyPin);

	std::pair<std::map<std::string, AddyPin>::iterator, bool> ret;
	ret = mPinMap.insert(std::pair<std::string, AddyPin>(addyPin.GetPin(), addyPin));

	if (ret.second == false) {
		// pin already used
		printf("pin already used in the hash maps\n");
		DumpPinMap();
		return Add(user);
	} else {
		std::pair<std::map<std::string, AddyUserInfo&>::iterator, bool> ret2;
		ret2 = mUserInfoMap.insert(std::pair<std::string, AddyUserInfo&>(addyPin.GetPin(), user));
		if (ret2.second == false) {
			printf("insertion FAILED, should never occur!!\n");
			return addyPin;
		}
		SaveMap();
		return addyPin;
	}
}

bool AddyDB::SaveMap() {

	if (!mHdfManager.IsCreated()) {
		printf("SaveMap: file not created, create it now\n");
		mHdfManager.Create();
	}

	mHdfManager.SetFieldValue(mUserInfoMap.size(), "numEntries");
	int i = 0;
	for (std::map<std::string, AddyUserInfo&>::iterator it = mUserInfoMap.begin(); it != mUserInfoMap.end(); it++, i++) {
		mHdfManager.SetFieldText(it->second.Serialize(it->first), "entry_%d", i);
	}

	mHdfManager.ExportData(true);
	return true;
}

bool AddyDB::LoadMap() {
	if (mHdfManager.IsFilePresent()) {
		if (!mHdfManager.IsCreated()) {
			printf("LoadMap: file not created, create it now\n");
			mHdfManager.Create();
		}
		mHdfManager.ImportData();
		int32 totalEntries  = mHdfManager.GetFieldValue("numEntries");
		for (int i = 0; i < totalEntries; i++) {
			AddyUserInfo* pUi = new AddyUserInfo();
			std::string entry = mHdfManager.GetFieldText("entry_%d", i);
			std::string pin = pUi->Deserialize(entry);
			std::pair<std::map<std::string, AddyUserInfo&>::iterator, bool> ret = mUserInfoMap.insert(std::pair<std::string, AddyUserInfo&>(pin, *pUi));
			if (ret.second == false) {
				printf("LoadMap: insertion FAILED, should never occur!!\n");
				return false;
			}
		}
	}
	return true;
}

void AddyDB::DumpPinMap() {
	printf("==================\n");
	for (std::map<std::string, AddyPin>::iterator it = mPinMap.begin(); it != mPinMap.end(); it++) {
		printf("pin map entry with key @ %p = %s and data @ %p\n", &(it->first), it->first.c_str(), &(it->second));
	}
	printf("==================\n");
}

AddyDB::ELookupResult AddyDB::FindByPin(std::string pin, AddyUserInfo& rRet) {
	ELookupResult ret = kNotFound;
	if (!mAddyPinAllocator.ValidatePin(pin)) {
		ret = kInvalidPin;
	}
	if (mUserInfoMap.count(pin) != 0) {
		std::map<std::string, AddyUserInfo&>::iterator it = mUserInfoMap.find(pin);
		rRet = it->second;
		ret = kSuccess;
	}

	return ret;
}

AddyUserInfo& AddyDB::CreateUserInfo(std::string addr, std::string name) {
	AddyUserInfo* pUser = new AddyUserInfo(addr, name);
	return *pUser;
}
