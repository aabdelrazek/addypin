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

std::string& AddyPin::GetPin() {
	return mPin;
}

void AddyPin::SetPin(std::string val) {
	mPin = val;
}
