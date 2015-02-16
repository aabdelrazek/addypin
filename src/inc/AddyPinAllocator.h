/*
 * AddyPinAllocator.h
 *
 *  Created on: 2015-02-01
 *      Author: aabdelrazek
 */

#ifndef ADDYPINALLOCATOR_H_
#define ADDYPINALLOCATOR_H_

#include <string>

//! this class takes care of creating new pin for a new user using some random generators
class AddyPinAllocator {
public:
	AddyPinAllocator();
	virtual ~AddyPinAllocator();

	std::string AllocatePin();
	bool ValidatePin(const std::string& val);

	std::string AllocateMasterPin();
	bool ValidateMasterPin(const std::string& val);

	static unsigned int ValidPinLength() {return 6;}
	static unsigned int ValidMasterPinLength() {return 8;}

private:
	bool ValidNumeric(const char& c);
	bool ValidChar(const char& c);
	static const int kMinNumAsccii = 48; 			// '0'
	static const int kMaxNumAscii = 57;				// '9'
	static const int kMinCharAsccii = 65; 			// 'A'
	static const int kMaxCharAsccii = 90; 			// 'Z'
	static const int kMinCharAscciiSmall = 97; 		// 'a'
	static const int kMaxCharAscciiSmall = 122; 	// 'z'

};

#endif /* ADDYPINALLOCATOR_H_ */
