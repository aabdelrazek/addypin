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
#include "inc/AddyInfoDialog.h"
#include <string>
#include <Wt/WLogger>

using namespace Wt;

AddyDB* sharedDB = NULL;

WApplication* createApplication(const WEnvironment& env)
{
	WApplication* pApp = NULL;

	printf("\n.............NEW APP.............\n");
	if (env.hostName().length() == AddyPinAllocator::ValidPinLength() + std::string(".addypin.com").length()) {
		pApp = new AddyPinApp(env, *sharedDB, AddyPinApp::kLookupView,
				env.hostName().substr(0, AddyPinAllocator::ValidPinLength()));
	} else if (env.hostName() == std::string("addypin.com") || env.hostName() == std::string("www.addypin.com")) {
			pApp = new AddyPinApp(env, *sharedDB, AddyPinApp::kMainview);
	} else if (env.hostName().length() == AddyPinAllocator::ValidMasterPinLength() + std::string(".addypin.com").length()) {
		pApp = new AddyPinApp(env, *sharedDB, AddyPinApp::kAccountManagementView,
				env.hostName().substr(0, AddyPinAllocator::ValidMasterPinLength()));
	}

	return pApp;
}

int main(int argc, char **argv)
{
	// Setup the logger
	WLogger logger;
	logger.addField("datetime", false);
	logger.addField("session", false);
	logger.addField("type", false);
	logger.addField("message", true);
	logger.setFile("AddyDB.log");

	sharedDB = new AddyDB(logger);
	WRun(argc, argv, &createApplication);
}
