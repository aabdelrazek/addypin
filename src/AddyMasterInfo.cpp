/*
 * AddyMasterInfo.cpp
 *
 *  Created on: 2015-02-14
 *      Author: aabdelrazek
 */

#include "inc/AddyMasterInfo.h"
#include "utils/inc/strtk.hpp"
#include <Wt/WApplication>
#include "tinyxml/tinyxml.h"


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

void AddyMasterInfo::Serialize(TiXmlElement* pNode) {
	TiXmlElement* pEmail = new TiXmlElement("email");
	pEmail->LinkEndChild(new TiXmlText(mEmail));
	pNode->LinkEndChild(pEmail);

	TiXmlElement* pMasterPin = new TiXmlElement("user_pin");
	pMasterPin->LinkEndChild(new TiXmlText(mMasterPin));
	pNode->LinkEndChild(pMasterPin);

	TiXmlElement* pAddresses = new TiXmlElement("addresses");
	pNode->LinkEndChild(pAddresses);
	TiXmlElement* pNumEntries = new TiXmlElement("num_addresses");
	char nn[10];
	sprintf(nn, "%lu", mUserInfoEntries.size());
	pNumEntries->LinkEndChild(new TiXmlText(nn));
	pAddresses->LinkEndChild(pNumEntries);

	for (std::vector<AddyUserInfo*>::iterator it = mUserInfoEntries.begin(); it != mUserInfoEntries.end(); it++) {
		TiXmlElement* pEntry = new TiXmlElement("entry");
		pAddresses->LinkEndChild(pEntry);
		(*it)->Serialize(pEntry);
	}
}

void AddyMasterInfo::Deserialize(TiXmlElement* pNode) {

	TiXmlElement* pEmail = pNode->FirstChildElement("email");
	mEmail = pEmail->FirstChild()->Value();

	TiXmlElement* pMasterPin = pNode->FirstChildElement("user_pin");
	mMasterPin = pMasterPin->FirstChild()->Value();

	TiXmlElement* pAddresses = pNode->FirstChildElement("addresses");

	TiXmlElement* pNumEntries = pAddresses->FirstChildElement("num_addresses");
	unsigned int numEntries = 0;
	if (pNumEntries) {
		sscanf(pNumEntries->FirstChild()->Value(), "%d", &numEntries);
	}

	TiXmlElement* entry = pAddresses->FirstChildElement("entry");
	for (unsigned int i = 0; i < numEntries; i++) {
		AddyUserInfo* pNew = new AddyUserInfo();
		pNew->Deserialize(entry);
		mUserInfoEntries.push_back(pNew);
		entry = entry->NextSiblingElement("entry");
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
