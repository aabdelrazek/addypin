/*
 * AddyPin.cpp
 *
 *  Created on: 2015-02-01
 *      Author: aabdelrazek
 */

#include "inc/AddyPin.h"

AddyPin::AddyPin() {
}

AddyPin::~AddyPin() {
}

const std::string& AddyPin::GetPin() {
	return mPin;
}

void AddyPin::SetPin(std::string& val) {
	mPin = val;
}

void AddyPin::SetPin(const char* val) {
	mPin = std::string(val);
}
