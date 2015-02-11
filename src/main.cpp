/*
 * main.cpp
 *
 *  Created on: 2015-01-31
 *      Author: aabdelrazek
 */

#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include "inc/AddyPinApp.h"
#include "inc/AddyPin.h"
#include "inc/AddyDB.h"
#include "inc/AddyUserInfo.h"
#include <string>

using namespace Wt;

WApplication* createApplication(const WEnvironment& env)
{
	AddyDB* db = new AddyDB();
	WApplication* pApp = NULL;
	std::string expectedPin = "";
	AddyUserInfo ret("", "");
	AddyDB::ELookupResult res;

	if (env.hostName().length() == AddyPin::PinLength() + std::string(".addypin.com").length()) {
		expectedPin = env.hostName().substr(0, AddyPin::PinLength());
		res = db->FindByPin(expectedPin, ret);
		pApp = new AddyPinApp(env, *db, AddyPinApp::kLookupView, res, &ret);
	} else if (env.hostName() == std::string("addypin.com") || env.hostName() == std::string("www.addypin.com")) {
		pApp = new AddyPinApp(env, *db, AddyPinApp::kMainview);
	} else {
		pApp = new WApplication(env);
	}

	return pApp;
}

int main(int argc, char **argv)
{
	return WRun(argc, argv, &createApplication);
}
