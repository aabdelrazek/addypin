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

const string AddyDB::kDbPath = "./";
const string AddyDB::kDbFile = "addypin_data.hdf";

static std::string LowerCase(std::string s) {
	string sLC;
	sLC.resize(s.size());
	transform(s.begin(), s.end(), sLC.begin(), ::tolower);
	return sLC;
}

static std::string UpperCase(std::string s) {
	string sLC;
	sLC.resize(s.size());
	transform(s.begin(), s.end(), sLC.begin(), ::toupper);
	return sLC;
}


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

string AddyDB::AllocateUniquePin(bool master) {
	string pin = "";
	// allocate a new unique pin
	if (master) {
		do {
			pin = mAddyPinAllocator.AllocateMasterPin();
		} while (mPinAddressMap.count(pin) != 0);
	} else {
		do {
			pin = mAddyPinAllocator.AllocatePin();
		} while (mMPinToInfoListMap.count(pin) != 0);
	}
	return pin;
}


AddyDB::EOperationResult AddyDB::Add(string address, string email, string& rAssignedPin) {
	AddyDB::EOperationResult ret = kSuccess;
	string pin = AllocateUniquePin(false);
	string pinLC = LowerCase(pin);
	string emailLC = LowerCase(email);
	string masterPin = "";
	AddyMasterInfo* pMasterRecord = NULL;
	// if email is first time here, assign it a master pin as well
	if (mEmailToMPinMap.count(emailLC) == 0) {
		masterPin = AllocateUniquePin(true);
		string masterPinLC = LowerCase(masterPin);
		mEmailToMPinMap[emailLC] = masterPinLC;
		pMasterRecord = new AddyMasterInfo(emailLC, masterPinLC);
		mMPinToInfoListMap[masterPin] = pMasterRecord;
		ret = kSuccessNewUse;
	} else {
		masterPin = mEmailToMPinMap[emailLC];
		pMasterRecord = mMPinToInfoListMap[masterPin];
	}

	AddyUserInfo* pNewEntry = new AddyUserInfo(address, pinLC);
	if (pMasterRecord->AddNewEntry(pNewEntry)) {
		mPinAddressMap[pinLC] = pNewEntry;
		SaveMap();
		rAssignedPin = pin;
	} else {
		// adding new entry failed, same user exceeded max number of entries!
		delete pNewEntry;
		ret = kExceededLimit;
	}

	return ret;
}

/*!
 * save data base from memory to file system
 */
bool AddyDB::SaveMap() {

	if (!mHdfManager.IsCreated()) {
		Wt::log("debug")<<"SaveMap: file not created, create it now";
		mHdfManager.Create();
	}

	mHdfManager.SetFieldValue(mMPinToInfoListMap.size(), "numEntries");
	int i = 0;
	for (map<string, AddyMasterInfo*>::iterator it = mMPinToInfoListMap.begin(); it != mMPinToInfoListMap.end(); it++, i++) {
		string str = "";
		it->second->Serialize(str);
		mHdfManager.SetFieldText(str, "e%d", i);
	}

	mHdfManager.ExportData(true);
	return true;
}

/*!
 * load data base from file system into memory
 */
bool AddyDB::LoadMap() {
	Wt::log("debug")<<"Loading DB file!!";
	if (mHdfManager.IsFilePresent()) {
		if (!mHdfManager.IsCreated()) {
			mHdfManager.Create();
		}
		mHdfManager.ImportData();
		unsigned int totalEntries  = mHdfManager.GetFieldValue("numEntries");
		for (unsigned int i = 0; i < totalEntries; i++) {
			AddyMasterInfo* pMasterRecord = new AddyMasterInfo();
			string entry = mHdfManager.GetFieldText("e%d", i);
			pMasterRecord->Deserialize(entry);
			pair<map<string, AddyMasterInfo*>::iterator, bool> ret;
			ret = mMPinToInfoListMap.insert(pair<string, AddyMasterInfo*>(pMasterRecord->GetMasterPin(), pMasterRecord));
			if (ret.second == false) {
				Wt::log("error")<<"LoadMap: insertion FAILED, should never occur!!";
				return false;
			} else {
				// let's update other maps as well.
				mEmailToMPinMap[pMasterRecord->GetEmail()] = pMasterRecord->GetMasterPin();
				for (unsigned int i = 0; i < pMasterRecord->GetNumEntries(); i++) {
					mPinAddressMap[pMasterRecord->GetEntry(i)->GetPin()] = pMasterRecord->GetEntry(i);
				}
			}
		}
	} else {
		Wt::log("debug")<<"no DB file!!";
	}
	return true;
}

void AddyDB::DumpPinMap() {
	Wt::log("debug")<<"=======================";
	for (map<string, AddyMasterInfo*>::iterator it = mMPinToInfoListMap.begin(); it != mMPinToInfoListMap.end(); it++) {
		Wt::log("debug")<<"pin map entry with key @ "<< &(it->first) <<"= " << it->first.c_str() << "and data @ " << &(it->second);
	}
	Wt::log("debug")<<"=======================";
}

AddyDB::EOperationResult AddyDB::FindByPin(string pin, AddyUserInfo*& pRet) {
	EOperationResult ret = kNotFound;
	std::string pinLC = LowerCase(pin);

	if (!mAddyPinAllocator.ValidatePin(pinLC)) {
		ret = kInvalidPin;
	}
	if (mPinAddressMap.count(pinLC) != 0) {
		map<string, AddyUserInfo*>::iterator it = mPinAddressMap.find(pinLC);
		pRet = it->second;
		ret = kSuccess;
	}

	return ret;
}

/*!
 * return a list of pairs of <pin, address>
 */
AddyDB::EOperationResult AddyDB::GetMasterRecord(string masterPin, string email, list< pair<string, string> >& retPairs) {
	AddyDB::EOperationResult ret = kSuccess;
	std::string emailLC = LowerCase(email);
	if (mEmailToMPinMap.count(emailLC) != 0 && mEmailToMPinMap[emailLC] == masterPin) {
		AddyMasterInfo* pInfo = mMPinToInfoListMap[masterPin];
		retPairs.clear();
		for (unsigned int i = 0; i < pInfo->GetNumEntries(); i++) {
			AddyUserInfo* pEntry = pInfo->GetEntry(i);
			retPairs.push_back(pair<string, string>(UpperCase(pEntry->GetPin()), pEntry->GetAddress()));
		}

	} else {
		ret = kNotFound;
	}

	return ret;
}

/*!
 * set user addresses/pins with the given list of pairs of <pin, address>
 */
AddyDB::EOperationResult AddyDB::SetMasterRecord(string masterPin, string email, list< pair<string, string> >& newPairs) {
	AddyDB::EOperationResult ret = kSuccess;
	std::string emailLC = LowerCase(email);
	if (mEmailToMPinMap.count(emailLC) != 0 && mEmailToMPinMap[emailLC] == masterPin) {
		AddyMasterInfo* pInfo = mMPinToInfoListMap[masterPin];

		// remove pin/address pairs of this master record from the map first
		for (unsigned int i = 0; i < pInfo->GetNumEntries(); i++) {
			AddyUserInfo* pUi = pInfo->GetEntry(i);
			map<string, AddyUserInfo*>::iterator it = mPinAddressMap.find(pUi->GetPin());
			delete pUi;
			mPinAddressMap.erase(it);
		}
		// delete all entries in this master record
		pInfo->DeleteAllEntries();

		// now iterate through the new set of pairs, and add them to both maps
		list< pair<string, string> >::iterator it;
		for (it = newPairs.begin(); it != newPairs.end(); it++) {
			AddyUserInfo* pEntry = new AddyUserInfo(it->second, it->first);
			mPinAddressMap[it->first] = pEntry;
			pInfo->AddNewEntry(pEntry);
		}

	} else {
		ret = kNotFound;
	}

	return ret;
}

AddyDB::EOperationResult AddyDB::GetMasterPin(std::string email, std::string& retMPin) {
	AddyDB::EOperationResult ret = kNotFound;
	std::string emailLC = LowerCase(email);
	if (mEmailToMPinMap.count(emailLC) != 0) {
		retMPin = mEmailToMPinMap[emailLC];
		ret = kSuccess;
	}
	return ret;
}
