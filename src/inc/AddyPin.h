/*
 * AddyPin.h
 *
 *  Created on: 2015-02-01
 *      Author: aabdelrazek
 */

#ifndef ADDYPIN_H_
#define ADDYPIN_H_
#include <string>

//! the addypin is supposed to be a 6 digits string that contains a mix of alphabetics and numerics and not a case sensitive!
class AddyPin {
public:
	AddyPin();
	virtual ~AddyPin();

	const std::string& GetPin();
	void SetPin(std::string& val);
	void SetPin(const char* val);
	unsigned int PinLength() { return mPin.length();}
private:
	std::string mPin;
};

#endif /* ADDYPIN_H_ */
