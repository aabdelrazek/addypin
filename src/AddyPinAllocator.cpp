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

std::string AddyPinAllocator::AllocatePin() {
	std::string str = "000000";
	int minNumAsccii = 48; // '0'
	int minCharAsccii = 97; // 'a'

	str[0] = rand() % 10 + minNumAsccii;
	str[1] = rand() % 26 + minCharAsccii;
	str[2] = rand() % 26 + minCharAsccii;
	str[3] = rand() % 26 + minCharAsccii;
	str[4] = rand() % 10 + minNumAsccii;
	str[5] = rand() % 10 + minNumAsccii;

	return str;
}


bool AddyPinAllocator::ValidatePin(std::string val) {
	// length has to be 6
	return (val.length() == 6 && ValidNumeric(val[0]) && ValidNumeric(val[4]) && ValidNumeric(val[5]) && ValidChar(val[1]) && ValidChar(val[2]) && ValidChar(val[3]));
}


bool AddyPinAllocator::ValidNumeric(char& c) {
	return (c >= 48 && c <= 57);
}
bool AddyPinAllocator::ValidChar(char& c) {
	return (c >= 97 && c <= 122) || (c >= 65 && c <= 90);
}
