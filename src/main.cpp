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

	if (env.hostName().length() == AddyPinAllocator::ValidPinLength() + std::string(".addypin.com").length()) {
		pApp = new AddyPinApp(env,
				*db, AddyPinApp::kLookupView,
				env.hostName().substr(0, AddyPinAllocator::ValidPinLength()));

	} else if (env.hostName() == std::string("addypin.com") || env.hostName() == std::string("www.addypin.com")) {
		pApp = new AddyPinApp(env,
				*db,
				AddyPinApp::kMainview);

	} else if (env.hostName().length() == AddyPinAllocator::ValidMasterPinLength() + std::string(".addypin.com").length()) {
		pApp = new AddyPinApp(env,
				*db, AddyPinApp::kAccountManagementView,
				env.hostName().substr(0, AddyPinAllocator::ValidMasterPinLength()));
	}

	return pApp;
}

int main(int argc, char **argv)
{
	return WRun(argc, argv, &createApplication);
}
