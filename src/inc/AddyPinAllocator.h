/*
 * AddyPinAllocator.h
 *
 *  Created on: 2015-02-01
 *      Author: aabdelrazek
 */

#ifndef ADDYPINALLOCATOR_H_
#define ADDYPINALLOCATOR_H_

#include <string>
#include "AddyUserInfo.h"
#include "AddyPin.h"

//! this class takes care of creating new pin for a new user using some random generators
class AddyPinAllocator {
public:
	AddyPinAllocator();
	virtual ~AddyPinAllocator();

	void AllocatePin(AddyPin& rNewPin);
	bool ValidatePin(std::string val);
private:
	bool ValidNumeric(char& c);
	bool ValidChar(char& c);
};

#endif /* ADDYPINALLOCATOR_H_ */
