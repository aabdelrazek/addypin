/*
 * AddyAccountMngmnt.cpp
 *
 *  Created on: 2015-02-15
 *      Author: aabdelrazek
 */

#include "inc/AddyAccountMngmnt.h"
#include <Wt/WTemplate>
#include <Wt/Mail/Client>
#include <Wt/Mail/Message>
#include <Wt/WTable>
#include <Wt/WBootstrapTheme>
#include <Wt/WIconPair>
#include <Wt/WImage>
#include <Wt/WFileResource>
#include <Wt/WCheckBox>
#include <Wt/WTextArea>

AddyAccountMngmnt::AddyAccountMngmnt(Wt::WContainerWidget* pContainer, AddyDB& rDB) :
		mrDB(rDB),
		mpInputMasterPin(NULL),
		mpInputEmail(NULL),
		mpAccountButton(NULL),
		mpTable(NULL),
		mpContainer(pContainer),
		mppCheckDelete(NULL),
		mppAddresses(NULL) {

	mpTable = new Wt::WTable(mpContainer);
	mpTable->setStyleClass("table table-condensed");

	mpTable->elementAt(0, 0)->addWidget(new WText("Master Pin:"));
	mpTable->elementAt(0, 0)->setStyleClass("label");

	mpInputMasterPin = new WLineEdit(mpContainer);
	//mpInputMasterPin->setMinimumSize(350, 20);
	mpTable->elementAt(0, 1)->addWidget(mpInputMasterPin);
	mpTable->elementAt(0, 1)->setStyleClass("info");

	mpTable->elementAt(1, 0)->addWidget(new WText("email:"));
	mpTable->elementAt(1, 0)->setStyleClass("label");

	mpInputEmail = new WLineEdit(mpContainer);
	//mpInputEmail2->setMinimumSize(350, 20);
	mpTable->elementAt(1, 1)->addWidget(mpInputEmail);
	mpTable->elementAt(1, 1)->setStyleClass("info");

	mpAccountButton = new WPushButton("Manage My Addresses!", mpContainer);
	mpAccountButton->clicked().connect(this, &AddyAccountMngmnt::ManageAddress);

}

void AddyAccountMngmnt::ManageAddress() {
	if (mpAccountButton->text() == "Manage My Addresses!") {
		if (!mpInputMasterPin->text().empty() && !mpInputEmail->text().empty()) {
			mrDB.GetMasterRecord(mpInputMasterPin->text().narrow(), mpInputEmail->text().narrow(), mPairs);
			if (mPairs.size() > 0) {
				mppCheckDelete = new WCheckBox*[mPairs.size()];
				mppAddresses = new WTextArea*[mPairs.size()];

				Wt::WTable* pTable = new Wt::WTable(mpContainer);
				pTable->setStyleClass("table table-condensed");
				list< pair<string, string> >::iterator it;
				pTable->setHeaderCount(1);
				pTable->elementAt(0, 0)->addWidget(new Wt::WText("delete"));
				pTable->elementAt(0, 1)->addWidget(new Wt::WText("AddyPin"));
				pTable->elementAt(0, 2)->addWidget(new Wt::WText("Address"));
				unsigned int i = 1;
				for (it = mPairs.begin(); it != mPairs.end(); it++, i++) {
					mppCheckDelete[i-1] = new WCheckBox(mpContainer);
					mppAddresses[i-1] = new WTextArea(it->second, mpContainer);
					pTable->elementAt(i, 0)->addWidget(mppCheckDelete[i-1]);
					pTable->elementAt(i, 1)->addWidget(new WText(it->first, mpContainer));
					pTable->elementAt(i, 2)->addWidget(mppAddresses[i-1]);
					pTable->elementAt(i, 2)->setStyleClass("info");
					mpAccountButton->setText("Save Changes!");
				}
				mpTable->elementAt(2, 0)->addWidget(new WText("Your Addresses:", mpContainer));
				mpTable->elementAt(2, 0)->setStyleClass("label");
				mpTable->elementAt(2, 1)->addWidget(pTable);
				mpTable->elementAt(2, 1)->setStyleClass("info");
			} else {
				// invalid account link!
			}
		} else {
			// empty fields
		}
	} else {
		list< pair<string, string> > newPairs;
		list< pair<string, string> >::iterator it;
		int i = 0;
		for (it = mPairs.begin(); it != mPairs.end(); it++, i++) {
			if (!mppCheckDelete[i]->isChecked()) {
				newPairs.push_back(pair<string, string>(it->first, mppAddresses[i]->text().narrow()));
			} else {
				// deleted entry
				printf("deleted %s\n", it->second.c_str());
			}
		}
		mrDB.SetMasterRecord(mpInputMasterPin->text().narrow(), mpInputEmail->text().narrow(), newPairs);
		mrDB.SaveMap();
		mpTable->deleteRow(2);
		mpAccountButton->setText("Manage My Addresses!");
	}
}

