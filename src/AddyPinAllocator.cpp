/*
 * AddyPinAllocator.cpp
 *
 *  Created on: 2015-02-01
 *      Author: aabdelrazek
 */

#include "inc/AddyPinAllocator.h"
#include <stdlib.h>
#include <time.h>

AddyPinAllocator::AddyPinAllocator() {
	srand(time(NULL));
}

AddyPinAllocator::~AddyPinAllocator() {
}

/*!
 * allocate a 6 digits pin with the form "NCCCNN"
 */
std::string AddyPinAllocator::AllocatePin() {
	std::string str = "000000";

	str[0] = rand() % 10 + kMinNumAsccii;
	str[1] = rand() % 26 + kMinCharAsccii;
	str[2] = rand() % 26 + kMinCharAsccii;
	str[3] = rand() % 26 + kMinCharAsccii;
	str[4] = rand() % 10 + kMinNumAsccii;
	str[5] = rand() % 10 + kMinNumAsccii;

	return str;
}

/*!
 * validate a pin (has to be 6 digits and with the form "NCCCNN")
 */
bool AddyPinAllocator::ValidatePin(const std::string& val) {
	// length has to be 6
	return (val.length() == 6 &&
			ValidNumeric(val[0]) &&
			ValidNumeric(val[4]) &&
			ValidNumeric(val[5]) &&
			ValidChar(val[1]) &&
			ValidChar(val[2]) &&
			ValidChar(val[3]));
}

/*!
 * allocate a 8 digits master pin with the form "NNCCCCNN"
 */
std::string AddyPinAllocator::AllocateMasterPin() {
	std::string str = "00000000";

	str[0] = rand() % 10 + kMinNumAsccii;
	str[1] = rand() % 10 + kMinNumAsccii;
	str[2] = rand() % 26 + kMinCharAsccii;
	str[3] = rand() % 26 + kMinCharAsccii;
	str[4] = rand() % 26 + kMinCharAsccii;
	str[5] = rand() % 26 + kMinCharAsccii;
	str[6] = rand() % 10 + kMinNumAsccii;
	str[7] = rand() % 10 + kMinNumAsccii;

	return str;
}

/*!
 * validate a master pin (has to be 8 digits and with the form "NNCCCCNN")
 */
bool AddyPinAllocator::ValidateMasterPin(const std::string& val) {
	// length has to be 8
	return (val.length() == 8 &&
			ValidNumeric(val[0]) &&
			ValidNumeric(val[1]) &&
			ValidNumeric(val[6]) &&
			ValidNumeric(val[7]) &&
			ValidChar(val[2]) &&
			ValidChar(val[3]) &&
			ValidChar(val[4]) &&
			ValidChar(val[5]));
}

/*!
 * validate the given character is a numeric (0-9)
 */
bool AddyPinAllocator::ValidNumeric(const char& c) {
	return (c >= kMinNumAsccii && c <= kMaxNumAscii);
}

/*!
 * validate the given character is a char (A-Z)
 */
bool AddyPinAllocator::ValidChar(const char& c) {
	return (c >= kMinCharAsccii && c <= kMaxCharAsccii) || (c >= kMinCharAscciiSmall && c <= kMaxCharAscciiSmall);
}
