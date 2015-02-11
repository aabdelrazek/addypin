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
#include <Wt/WApplication>

const std::string AddyDB::kDbPath = "./";
const std::string AddyDB::kDbFile = "addypin_data.hdf";

AddyDB::AddyDB() :
		mHdfFileAdapter(kDbPath+kDbFile),
		mHdfManager(&mHdfFileAdapter) {
	// intentionally left blank
	try {
		LoadMap();
	} catch (CException& e) {
		Wt::log("error")<<"no data to load!!!!!";
	}
}

AddyDB::~AddyDB() {
	try {
		SaveMap();
	}catch (CException& e) {
		Wt::log("error")<<"Failed to save!!!!!";
	}
}

std::string AddyDB::Add(AddyUserInfo& user) {

	std::string pin = mAddyPinAllocator.AllocatePin();
	std::pair<std::map<std::string, AddyUserInfo&>::iterator, bool> ret;
	ret = mUserInfoMap.insert(std::pair<std::string, AddyUserInfo&>(pin, user));

	if (ret.second == false) {
		// pin already used
		Wt::log("error")<< "pin already used in the hash maps";
		DumpPinMap();
		return Add(user);
	} else {
		SaveMap();
		return pin;
	}
}

bool AddyDB::SaveMap() {

	if (!mHdfManager.IsCreated()) {
		Wt::log("debug")<<"SaveMap: file not created, create it now";
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
				Wt::log("error")<<"LoadMap: insertion FAILED, should never occur!!";
				return false;
			}
		}
	}
	return true;
}

void AddyDB::DumpPinMap() {
	Wt::log("debug")<<"=======================";
	for (std::map<std::string, AddyUserInfo&>::iterator it = mUserInfoMap.begin(); it != mUserInfoMap.end(); it++) {
		Wt::log("debug")<<"pin map entry with key @ "<< &(it->first) <<"= " << it->first.c_str() << "and data @ " << &(it->second);
	}
	Wt::log("debug")<<"=======================";
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
