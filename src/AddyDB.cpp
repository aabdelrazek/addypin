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
#include <Wt/WLogger>
#include "tinyxml/tinyxml.h"

const string AddyDB::kDbPath = "./";
const string AddyDB::kDbFile = "addypin_data.xml";

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

AddyDB::AddyDB(Wt::WLogger& rLogger) :
		mrLog(rLogger) {
	// intentionally left blank
	try {
		LoadMap();
	} catch (CException& e) {
		mrLog.entry("error") <<"no data to load!!!!!";
	}
}

AddyDB::~AddyDB() {
	try {
		SaveMap();
	}catch (CException& e) {
		mrLog.entry("error")<<"Failed to save!!!!!";
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
	TiXmlDocument doc;

	TiXmlElement* pNumEntries = new TiXmlElement("num_users");

	char nn[10];
	sprintf(nn, "%lu", mMPinToInfoListMap.size());

	pNumEntries->LinkEndChild(new TiXmlText(nn));
	doc.LinkEndChild(pNumEntries);

	if (mMPinToInfoListMap.size() > 0) {
		TiXmlElement* pElements = new TiXmlElement("all_users");
		doc.LinkEndChild(pElements);
		for (map<string, AddyMasterInfo*>::iterator it = mMPinToInfoListMap.begin(); it != mMPinToInfoListMap.end(); it++) {
			TiXmlElement* pElement = new TiXmlElement("user");
			it->second->Serialize(pElement);
			pElements->LinkEndChild(pElement);
		}
	}
	return doc.SaveFile((kDbPath+kDbFile).c_str());
}

/*!
 * load data base from file system into memory
 */
bool AddyDB::LoadMap() {
	bool ret = false;
	TiXmlDocument doc((kDbPath+kDbFile).c_str());
	bool loadOkay = doc.LoadFile();
	if (!loadOkay) {
		Wt::log("error")<<"Could not load DB file, Error="<< doc.ErrorDesc() << "Exiting";
	}

	TiXmlElement* pNumEntries = doc.FirstChildElement("num_users");
	unsigned int numEntries = 0;
	if (pNumEntries) {
		sscanf(pNumEntries->FirstChild()->Value(), "%d", &numEntries);

		if (numEntries > 0) {
			TiXmlElement* pElements = doc.FirstChildElement("all_users");
			if (pElements) {
				TiXmlElement* pItterator = pElements->FirstChildElement("user");
				unsigned int i = 0;

				while (pItterator && i++ < numEntries) {
					AddyMasterInfo* pMasterRecord = new AddyMasterInfo();
					pMasterRecord->Deserialize(pItterator);
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
					pItterator = pItterator->NextSiblingElement("user");
				}
			}
		}
	}
	return ret;
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

	if (mEmailToMPinMap.count(emailLC) != 0) {
		if (mEmailToMPinMap[emailLC] == masterPin) {
			AddyMasterInfo* pInfo = mMPinToInfoListMap[masterPin];
			retPairs.clear();
			for (unsigned int i = 0; i < pInfo->GetNumEntries(); i++) {
				AddyUserInfo* pEntry = pInfo->GetEntry(i);
				retPairs.push_back(pair<string, string>(UpperCase(pEntry->GetPin()), pEntry->GetAddress()));
			}
		} else {
			ret = kInvalidPin;
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
			mPinAddressMap.erase(it);
			delete pUi;
		}
		// delete all entries in this master record
		pInfo->DeleteAllEntries();

		// special case when user deleted, all his addresses!
		if (newPairs.size() == 0) {
			// delete email to pin entry
			map<string, string>::iterator it = mEmailToMPinMap.find(emailLC);
			mEmailToMPinMap.erase(it);
			//delete pin to master info entry
			map<string, AddyMasterInfo*>::iterator it2 = mMPinToInfoListMap.find(masterPin);
			mMPinToInfoListMap.erase(it2);
			delete pInfo;
		} else {
			// now iterate through the new set of pairs, and add them to both maps
			list< pair<string, string> >::iterator it;
			for (it = newPairs.begin(); it != newPairs.end(); it++) {
				AddyUserInfo* pEntry = new AddyUserInfo(it->second, LowerCase(it->first));
				mPinAddressMap[LowerCase(it->first)] = pEntry;
				pInfo->AddNewEntry(pEntry);
			}
		}
		SaveMap();

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
