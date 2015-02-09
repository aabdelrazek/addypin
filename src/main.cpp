/*
 * main.cpp
 *
 *  Created on: 2015-01-31
 *      Author: aabdelrazek
 */

#include <Wt/WApplication>
#include "inc/AddyPinApp.h"
#include "inc/AddyDB.h"

using namespace Wt;

WApplication *createApplication(const WEnvironment& env)
{
	AddyDB* db = new AddyDB();

//	AddyUserInfo& ui = db->CreateUserInfo("add1", "name1");
//	db->Add(ui);

	return new AddyPinApp(env, *db);
}

int main(int argc, char **argv)
{
	return WRun(argc, argv, &createApplication);
}
