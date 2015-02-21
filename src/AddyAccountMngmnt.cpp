/*
 * AddyAccountMngmnt.cpp
 *
 *  Created on: 2015-02-15
 *      Author: aabdelrazek
 */

#include "inc/AddyAccountMngmnt.h"
#include "inc/AddyText.h"
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


AddyAccountMngmnt::AddyAccountMngmnt(Wt::WContainerWidget* pContainer, AddyDB& rDB, std::string masterPin) :
		mMasterPin(masterPin),
		mrDB(rDB),
		mpInputEmail(NULL),
		mpAccountButton(NULL),
		mpTable(NULL),
		mpContainer(pContainer),
		mppCheckDelete(NULL),
		mppAddresses(NULL) {

	mpTable = new Wt::WTable(mpContainer);
	mpTable->setStyleClass("table table-condensed");

	mpTable->elementAt(0, 0)->addWidget(new WText(kIdEmail));
	mpTable->elementAt(0, 0)->setStyleClass("label");

	mpInputEmail = new WLineEdit(mpContainer);
	mpTable->elementAt(0, 1)->addWidget(mpInputEmail);
	mpTable->elementAt(0, 1)->setStyleClass("info");

	mpAccountButton = new WPushButton(kIdMngAddresses, mpContainer);
	mpAccountButton->clicked().connect(this, &AddyAccountMngmnt::ManageAddress);
}

AddyAccountMngmnt::~AddyAccountMngmnt() {
	delete mpAccountButton;
	delete mpInputEmail;
	delete mpTable;
}

void AddyAccountMngmnt::ManageAddress() {
	if (mpAccountButton->text() == kIdMngAddresses) {
		if (!mpInputEmail->text().empty()) {
			if (mpInputEmail->text().toUTF8().find('@') != string::npos) {

				AddyDB::EOperationResult res = mrDB.GetMasterRecord(mMasterPin, mpInputEmail->text().toUTF8(), mPairs);
				switch (res) {
				case AddyDB::kSuccess:
					if (mPairs.size() > 0) {
						mppCheckDelete = new WCheckBox*[mPairs.size()];
						mppAddresses = new WTextArea*[mPairs.size()];

						Wt::WTable* pTable = new Wt::WTable(mpContainer);
						pTable->setStyleClass("table table-condensed");
						list< pair<string, string> >::iterator it;
						pTable->setHeaderCount(1);
						pTable->elementAt(0, 0)->addWidget(new Wt::WText(kIdDelete));
						pTable->elementAt(0, 1)->addWidget(new Wt::WText(kIdAddyPin));
						pTable->elementAt(0, 2)->addWidget(new Wt::WText(kIdAddr));
						unsigned int i = 1;
						for (it = mPairs.begin(); it != mPairs.end(); it++, i++) {
							mppCheckDelete[i-1] = new WCheckBox(mpContainer);
							mppAddresses[i-1] = new WTextArea(it->second, mpContainer);
							pTable->elementAt(i, 0)->addWidget(mppCheckDelete[i-1]);
							pTable->elementAt(i, 1)->addWidget(new WText(it->first, mpContainer));
							pTable->elementAt(i, 2)->addWidget(mppAddresses[i-1]);
							pTable->elementAt(i, 2)->setStyleClass("info");
							mpAccountButton->setText(kIdSaveChanges);
						}
						mpTable->elementAt(2, 0)->addWidget(new WText(kIdYourAddresses, mpContainer));
						mpTable->elementAt(2, 0)->setStyleClass("label");
						mpTable->elementAt(2, 1)->addWidget(pTable);
						mpTable->elementAt(2, 1)->setStyleClass("info");
					}
					break;
				case AddyDB::kInvalidPin:
					// invalid account link!
					mInfoDialog.Show(kIdError, kIdEmailLinkNotMatch);
					break;

				case AddyDB::kNotFound:
					mInfoDialog.Show(kIdError, kIdEmailNotExist);
					break;

				default:
					break;
				}
			} else {
				mInfoDialog.Show(kIdError, kIdInvalidEmail);
			}
		} else {
			mInfoDialog.Show(kIdError, kIdEmptyEmail);
		}
	} else {
		list< pair<string, string> > newPairs;
		list< pair<string, string> >::iterator it;
		int i = 0;
		for (it = mPairs.begin(); it != mPairs.end(); it++, i++) {
			if (!mppCheckDelete[i]->isChecked()) {
				newPairs.push_back(pair<string, string>(it->first, mppAddresses[i]->text().toUTF8()));
			} else {
				// user deleting an entry
			}
		}
		mrDB.SetMasterRecord(mMasterPin, mpInputEmail->text().toUTF8(), newPairs);
		mpTable->deleteRow(2);
		mpAccountButton->setText(kIdMngAddresses);
	}
}

