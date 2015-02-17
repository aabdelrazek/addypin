/*
 * AddyAccountMngmnt.h
 *
 *  Created on: 2015-02-15
 *      Author: aabdelrazek
 */

#ifndef ADDACCOUNTMNGMNT_H_
#define ADDACCOUNTMNGMNT_H_

#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WPanel>

#include "AddyDB.h"

using namespace Wt;

class AddyAccountMngmnt: public WObject {
public:
	AddyAccountMngmnt(Wt::WContainerWidget* pContainer, AddyDB& rDB, std::string masterPin);

private:
	std::string mMasterPin;
	AddyDB& mrDB;
	list< pair<string, string> > mPairs;//!< pin/address pairs
	WLineEdit* mpInputEmail; 			//!< UI for user to put his name
	WPushButton* mpAccountButton;		//!< To submit new entry and allocate a new AddyPin for it
	Wt::WTable* mpTable;				//!< table holding all UI elements of the account management
	Wt::WContainerWidget* mpContainer;	//!< top container
	Wt::WCheckBox** mppCheckDelete;		//!< check-boxes for entry deletion
	Wt::WTextArea** mppAddresses;		//!< table entries for addresses
    void ManageAddress();
};

#endif /* ADDACCOUNTMNGMNT_H_ */
