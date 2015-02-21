/*
 * AddyUserInfo.cpp
 *
 *  Created on: 2015-02-01
 *      Author: aabdelrazek
 */

#include "inc/AddyUserInfo.h"
#include "utils/inc/strtk.hpp"
#include "utils/inc/CException.hpp"
#include <Wt/WApplication>
#include "tinyxml/tinyxml.h"

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

void AddyUserInfo::Deserialize(TiXmlNode* entry) {
	try {
		TiXmlElement* pAddr = entry->FirstChildElement("addr");
		mAddress = pAddr->FirstChild()->Value();

		TiXmlElement* pPin = entry->FirstChildElement("pin");
		mPin.SetPin(pPin->FirstChild()->Value());

	} catch (CException& e) {
		printf("%s\n", e.what());
	}
}

/*!
 * encode the user info in 1 string to store to database file and include the given AddyPin in the entry
 */
void AddyUserInfo::Serialize(TiXmlNode* pEntry) {
	try {
			TiXmlElement* pAddr = new TiXmlElement("addr");
			pAddr->LinkEndChild(new TiXmlText(mAddress));
			pEntry->LinkEndChild(pAddr);

			TiXmlElement* pPin = new TiXmlElement("pin");
			pPin->LinkEndChild(new TiXmlText(mPin.GetPin()));
			pEntry->LinkEndChild(pPin);

		} catch (CException& e) {
		printf("%s\n", e.what());
	}
}
