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


AddyDB::EOperationResult AddyDB::Add(string address, string email, string& rAssignedPin, string& rMasterPin) {
	AddyDB::EOperationResult ret = kSuccess;
	string pin = AllocateUniquePin(false);
	string masterPin = "";
	AddyMasterInfo* pMasterRecord = NULL;
	// if email is first time here, assign it a master pin as well
	if (mEmailToMPinMap.count(email) == 0) {
		masterPin = AllocateUniquePin(true);
		mEmailToMPinMap[email] = masterPin;
		pMasterRecord = new AddyMasterInfo(email, masterPin);
		mMPinToInfoListMap[masterPin] = pMasterRecord;
		ret = kSuccessNewUse;
	} else {
		masterPin = mEmailToMPinMap[email];
		pMasterRecord = mMPinToInfoListMap[masterPin];
	}

	AddyUserInfo* pNewEntry = new AddyUserInfo(address, pin);
	if (pMasterRecord->AddNewEntry(pNewEntry)) {
		mPinAddressMap[pin] = pNewEntry;
		SaveMap();
		rAssignedPin = pin;
		rMasterPin = masterPin;
	} else {
		// adding new entry failed, same user exceeded max number of entries!
		delete pNewEntry;
		ret = kExceededLimit;
		rMasterPin = masterPin;
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
	string pinLowerCase;
	pinLowerCase.resize(pin.size());
	transform(pin.begin(), pin.end(), pinLowerCase.begin(), ::toupper);

	if (!mAddyPinAllocator.ValidatePin(pinLowerCase)) {
		ret = kInvalidPin;
	}
	if (mPinAddressMap.count(pinLowerCase) != 0) {
		map<string, AddyUserInfo*>::iterator it = mPinAddressMap.find(pinLowerCase);
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
	if (mEmailToMPinMap.count(email) != 0 && mEmailToMPinMap[email] == masterPin) {
		AddyMasterInfo* pInfo = mMPinToInfoListMap[masterPin];
		retPairs.clear();
		for (unsigned int i = 0; i < pInfo->GetNumEntries(); i++) {
			AddyUserInfo* pEntry = pInfo->GetEntry(i);
			retPairs.push_back(pair<string, string>(pEntry->GetPin(), pEntry->GetAddress()));
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
	if (mEmailToMPinMap.count(email) != 0 && mEmailToMPinMap[email] == masterPin) {
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
