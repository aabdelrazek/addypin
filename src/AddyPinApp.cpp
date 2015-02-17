/*
 * AddyPinApp.cpp
 *
 *  Created on: 2015-02-01
 *      Author: aabdelrazek
 */

#include "inc/AddyPinApp.h"
#include "inc/AddyAccountMngmnt.h"
#include "inc/AddyInfoDialog.h"
#include "inc/AddyText.h"
#include <Wt/WTemplate>
#include <Wt/Mail/Client>
#include <Wt/Mail/Message>
#include <Wt/WTable>
#include <Wt/WBootstrapTheme>
#include <Wt/WIconPair>
#include <Wt/WImage>
#include <Wt/WFileResource>
#include <Wt/WContainerWidget>
#include <Wt/WMenuItem>
#include <Wt/WTabWidget>

/*
 * The env argument contains information about the new session, and
 * the initial request. It must be passed to the WApplication
 * constructor so it is typically also an argument for your custom
 * application constructor.
*/
AddyPinApp::AddyPinApp(const WEnvironment& env, AddyDB& rDB, AppStartupView view, std::string pinToLookup):
		WApplication(env),
		mStartupView(view),
		mrDB(rDB),
		mpContainer(NULL),
		mpInputAddress(NULL),
		mpInputEmail(NULL),
		mpSubmitButton(NULL),
		mpInputPin(NULL),
		mpLookupButton(NULL),
		mpInputEmail2(NULL),
		mpManageButton(NULL),
		mpAccountMngmnt(NULL),
		mpResult(NULL),
		mResultPanel(NULL) {

	setTitle("AddyPin!!");
    useStyleSheet("./res/CSSexample.css");
    setTheme(new WBootstrapTheme());

    // Addypin logo
    Wt::WImage *image = new Wt::WImage( Wt::WLink(new Wt::WFileResource("./res/logo.png")), root());
    image->setAlternateText("AddyPin logo");

    // couple lines break after the logo
    root()->addWidget(new WBreak());
    root()->addWidget(new WBreak());

    // the top widget container
    mpContainer = new Wt::WContainerWidget(root());
    //mpContainer->setStyleClass("AddyPinMain");

    // the tab widget
    mTabW = new Wt::WTabWidget(mpContainer);



    mpContainer->addWidget(new WBreak());
    mpContainer->addWidget(new WBreak());

        switch(mStartupView) {
		case kMainview:
			BuildMainView();
			break;

		case kAccountManagementView:
			mpAccountMngmnt = new AddyAccountMngmnt(new Wt::WContainerWidget(mpContainer), mrDB, pinToLookup);
			break;

		case kLookupView:
			std::string res = LookupAddress(pinToLookup);
			mpResult = new WText(new Wt::WContainerWidget(root()));
			mpResult->setText(res);
			break;
    }
}

void AddyPinApp::BuildMainView() {
    /* top container that includes:
     * 1 - new input widget
     * 2 - pin lookup widget
     * 3 - result */

    // new input widget
    Wt::WContainerWidget* pNewInputContainer = new Wt::WContainerWidget(mpContainer);
	mTabW->addTab(pNewInputContainer, kIdRegNewAddr, Wt::WTabWidget::PreLoading);

	Wt::WTable* pTable1 = new Wt::WTable(pNewInputContainer);
	pTable1->setStyleClass("table table-condensed");
	pTable1->elementAt(0, 0)->setStyleClass("label");
	pTable1->elementAt(0, 0)->addWidget(new WText(kIdDetailedAddr));

	mpInputAddress = new WTextArea(pNewInputContainer);
	mpInputAddress->setFocus();
	pTable1->elementAt(0, 1)->addWidget(mpInputAddress);
	pTable1->elementAt(0, 1)->setStyleClass("info");
	pTable1->elementAt(1, 0)->addWidget(new WText(kIdEmail));
	pTable1->elementAt(1, 0)->setStyleClass("label");
	mpInputEmail = new WLineEdit(pNewInputContainer);
	pTable1->elementAt(1, 1)->addWidget(mpInputEmail);
	pTable1->elementAt(1, 1)->setStyleClass("info");
	mpSubmitButton = new WPushButton(kIdSubmit, pNewInputContainer);
	mpSubmitButton->clicked().connect(this, &AddyPinApp::SubmitNewAddress);
	////////////////////////////////////////////////////////////////

	// lookup widget
	Wt::WContainerWidget* pLookupContainer = new Wt::WContainerWidget(mpContainer);
	mTabW->addTab(pLookupContainer , kIdLookupAddr, Wt::WTabWidget::PreLoading);
	Wt::WTable* pTable2 = new Wt::WTable(pLookupContainer);
	pTable2->setStyleClass("table table-condensed");

	pTable2->elementAt(0, 0)->addWidget(new WText(kIdAddyToLookup));
	pTable2->elementAt(0, 0)->setStyleClass("label");

	mpInputPin = new WLineEdit(pLookupContainer);
	pTable2->elementAt(0, 1)->addWidget(mpInputPin);
	pTable2->elementAt(0, 1)->setStyleClass("info");
	mpLookupButton = new WPushButton(kIdLookup, pLookupContainer);
	mpLookupButton->clicked().connect(this, &AddyPinApp::Lookup);
	////////////////////////////////////////////////////////////////

	// account management widget
	Wt::WContainerWidget* pAccountContainer = new Wt::WContainerWidget(mpContainer);
	mTabW->addTab(pAccountContainer , kIdMngAddresses, Wt::WTabWidget::PreLoading);
	pAccountContainer->addWidget(new WText(kIdAskForEmail));
	Wt::WTable* pTable3 = new Wt::WTable(pAccountContainer);
	pTable3->setStyleClass("table table-condensed");

	pTable3->elementAt(0, 0)->addWidget(new WText(kIdEmail));
	pTable3->elementAt(0, 0)->setStyleClass("label");

	mpInputEmail2 = new WLineEdit(pAccountContainer);
	pTable3->elementAt(0, 1)->addWidget(mpInputEmail2);
	pTable3->elementAt(0, 1)->setStyleClass("info");
	mpManageButton = new WPushButton(kIdSendLink, pAccountContainer);
	mpManageButton->clicked().connect(this, &AddyPinApp::Manage);
}
/*!
 * take the user given name and address and add them to the data base and assign a new AddyPin to the entry
 */
bool AddyPinApp::Assign(std::string address, std::string email, std::string& rPin) {
	bool ret = false;
	AddyDB::EOperationResult res = mrDB.Add(address, email, rPin);
	std::string plainBody;
	std::string htmlBody;
	switch (res) {
	case AddyDB::kSuccess:
		plainBody = std::string(kIdThankUPlain);
		plainBody = plainBody.append(rPin);
		plainBody = plainBody.append(kIdShareItPlain);
		plainBody = plainBody.append(rPin).append(".addypin.com\n");
		plainBody = plainBody.append(kIdEndOfMailPlain);

		htmlBody = std::string(kIdThankUHtml);
		htmlBody = htmlBody.append(rPin).append("</b></p>");
		htmlBody = htmlBody.append(kIdShareItHtml);
		htmlBody = htmlBody.append("<p><a href=\"").append(rPin).append(".addypin.com\">").append(rPin).append(".addypin.com</a></p>");
		htmlBody = htmlBody.append(kIdEndOfMailHtml);
		SendEmail(kIdYourNewPin, "AddyPin", email, plainBody, htmlBody);
		ret = true;
		break;

	case AddyDB::kSuccessNewUse:
		plainBody = std::string(kIdFrstThankUPlain);
		plainBody = plainBody.append(rPin);
		plainBody = plainBody.append(kIdShareItPlain);
		plainBody = plainBody.append(rPin).append(".addypin.com\n");
		//plainBody = plainBody.append("your master pin is ").append(masterPin).append(", you will need that to manage your addresses\n");
		plainBody = plainBody.append(kIdEndOfMailPlain);

		htmlBody = std::string(kIdFrstThankUHtml);
		htmlBody = htmlBody.append(rPin).append("</b></p>");
		htmlBody = htmlBody.append(kIdShareItHtml);
		htmlBody = htmlBody.append("<p><a href=\"").append(rPin).append(".addypin.com\">").append(rPin).append(".addypin.com</a></p>");
		//htmlBody = htmlBody.append("<p>your master pin is ").append(masterPin).append(", you will need that to manage your addresses</p>");
		htmlBody = htmlBody.append(kIdEndOfMailHtml);
		SendEmail(kIdYourNewPin, "AddyPin", email, plainBody, htmlBody);
		ret = true;
		break;

	case AddyDB::kExceededLimit:
		ret = false;
		break;

	default:
		break;
	}
	return ret;
}

void AddyPinApp::SendEmail(std::string subject, std::string from, std::string to, std::string plainBody, std::string htmlBody) {
	Mail::Message message;
	message.setFrom(Mail::Mailbox("admin@addypin.com", from));
	message.addRecipient(Mail::To, Mail::Mailbox(to, to));
	message.setSubject(subject);
	message.setBody(WString(plainBody));
	message.addHtmlBody(WString(htmlBody));

	Mail::Client client;
	client.connect();
	client.send(message);
}

/*!
 * handle submit key event new data to the data base, send email bacl to th user and show the new addypin
 */
void AddyPinApp::SubmitNewAddress() {
	std::string pin = "";
	if (!mpInputAddress->text().empty() && !mpInputEmail->text().empty()) {
		if (mpInputEmail->text().narrow().find('@') != string::npos) {
			if (Assign(mpInputAddress->text().narrow(), mpInputEmail->text().narrow(), pin)) {
				AddyInfoDialog info("Success", std::string("Your new Pin is ").append(pin.c_str()).append(", an email has been sent to ").append(mpInputEmail->text().narrow()));
			} else {
				AddyInfoDialog info(kIdFailed, kIdMaxLimit);
			}
		} else {
			AddyInfoDialog info(kIdError, kIdInvalidEmail);
		}
	} else {
		AddyInfoDialog info(kIdError, kIdEmptyField);
	}
}

/*!
 * take the user given AddyPin and lookup the corresponding info
 */
void AddyPinApp::Lookup() {
	std::string res = LookupAddress(mpInputPin->text().narrow());

	if (!mpResult) {
		mpResult = new WText(mpContainer);
	}
	mpResult->setText(res);
}

std::string AddyPinApp::LookupAddress(std::string pinToLookup) {
	AddyUserInfo* pRet = NULL;
	AddyDB::EOperationResult res = mrDB.FindByPin(pinToLookup, pRet);
	std::string retStr;
	switch(res) {
	case AddyDB::kSuccess:
		retStr = pRet->GetAddress();
		break;

	case AddyDB::kInvalidPin:
		retStr = std::string(kIdInvalidPin);
		break;

	case AddyDB::kNotFound:
		retStr = std::string(kIdEntryNotFound);
		break;

	default:
		break;
	}
	return retStr;
}

/*!
 * take the user given email and send him an email with account management link
 */
void AddyPinApp::Manage() {
	if (!mpInputEmail2->text().empty()) {
		std::string masterPin = "";
		AddyDB::EOperationResult ret = mrDB.GetMasterPin(mpInputEmail2->text().narrow(), masterPin);
		if (ret == AddyDB::kSuccess) {
			std::string plainBody;
			std::string htmlBody;
			plainBody = std::string(kIdReqReceivedPlain);
			plainBody = plainBody.append(kIdSecurityPlain);
			plainBody = plainBody.append(kIdWillSendYouPlain);
			plainBody = plainBody.append(masterPin).append(".addypin.com\n");
			plainBody = plainBody.append(kIdEndOfMailPlain);

			htmlBody = std::string("<p>").append(kIdReqReceivedPlain).append("</p>");
			htmlBody = htmlBody.append("<p>").append(kIdSecurityPlain).append("</p>");
			htmlBody = htmlBody.append("<p>").append(kIdWillSendYouPlain).append("</p>");
			htmlBody = htmlBody.append("<p><a href=\"").append(masterPin).append(".addypin.com\">").append(masterPin).append(".addypin.com</a></p>");
			htmlBody = htmlBody.append(kIdEndOfMailHtml);
			SendEmail(kIdYourMngmntLink, "AddyPin", mpInputEmail2->text().narrow(), plainBody, htmlBody);

			AddyInfoDialog info(kIdSuccess, kIdEmailSentMsg);
		} else if (ret == AddyDB::kNotFound) {
			AddyInfoDialog info(kIdError, kIdEmailNotExist);
		}
	} else {
		AddyInfoDialog info(kIdError, kIdEmptyEmail);
	}
}
