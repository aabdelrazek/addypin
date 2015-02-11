/*
 * AddyPinApp.h
 *
 *  Created on: 2015-02-01
 *      Author: aabdelrazek
 */

#ifndef ADDYPINAPP_H_
#define ADDYPINAPP_H_

#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WPanel>

#include "AddyDB.h"

using namespace Wt;


//! The top container of the AddyPin web application
class AddyPinApp: public WApplication
{
public:
	enum AppStartupView {
		kMainview = 0,
		kLookupView
	};

	AddyPinApp(const WEnvironment& env,
				AddyDB& rDB,
				AppStartupView view,
				AddyDB::ELookupResult res = AddyDB::kInvalidPin,
				AddyUserInfo* user = NULL);

private:
	AppStartupView	mStartupView;	//!< initial view of the web page, if accesses via a subdomain as a pin, will jumt to show result
	AddyDB& mrDB;					//!< reference to main Addy Pin Database

	// first UI elements for someone willing t create a new AddyPin for his address
    WLineEdit* mpInputAddress;		//!< UI for user to put address description or AddyPin for address lookup!
    WLineEdit* mpInputPin;			//!< UI for user to put a pin for lookup
    WLineEdit* mpNameEdit;			//!< UI for user to put his name
    WText* mpResult;				//!< UI to output to the user his assigned addypin or the address of a given AddyPin
    WPushButton* mpSubmitButton;	//!< To submit new entry and allocate a new AddyPin for it
    WPushButton* mpLookupButton;	//!< to lookup address of a given AddyPin
    Wt::WPanel *mResultPanel;
    std::string Assign(std::string address, std::string email);
    void Lookup();
    void Submit();
    void BuildMainView();
    void BuildLookupView(AddyDB::ELookupResult res, AddyUserInfo* user);
    void SendEmail(std::string subject, std::string from, std::string to, std::string plainBody, std::string htmlBody);
};

#endif /* ADDYPINAPP_H_ */
