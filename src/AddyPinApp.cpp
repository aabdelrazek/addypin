/*
 * AddyPinApp.cpp
 *
 *  Created on: 2015-02-01
 *      Author: aabdelrazek
 */

#include "inc/AddyPinApp.h"
#include "inc/AddyAccountMngmnt.h"
#include <Wt/WTemplate>
#include <Wt/Mail/Client>
#include <Wt/Mail/Message>
#include <Wt/WTable>
#include <Wt/WBootstrapTheme>
#include <Wt/WIconPair>
#include <Wt/WImage>
#include <Wt/WFileResource>

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
		mpInputAddress(NULL),
		mpInputEmail1(NULL),
		mpSubmitButton(NULL),
		mpInputPin(NULL),
		mpLookupButton(NULL),
		mpAccountMngmnt(NULL),
		mpResult(NULL),
		mResultPanel(NULL) {

	setTitle("AddyPin!!");
    useStyleSheet("./res/CSSexample.css");
    setTheme(new WBootstrapTheme());

    switch(mStartupView) {

    case kMainview:
    	BuildMainView();
    	break;

    case kAccountManagementView:
    	break;

    case kLookupView:
		AddyUserInfo* pRet = NULL;
		AddyDB::EOperationResult res = mrDB.FindByPin(pinToLookup, pRet);
    	Wt::WContainerWidget *pTopContainer = new Wt::WContainerWidget(root());
    	pTopContainer->setStyleClass("AddyPinMain");
    	mResultPanel = new Wt::WPanel(root());
    	mResultPanel->setTitle("Address Lookup Result!");
    	mResultPanel->setCentralWidget(pTopContainer);
    	mpResult = new WText(pTopContainer);
    	BuildLookupView(res, pRet);
    	break;
    }
}

void AddyPinApp::BuildLookupView(AddyDB::EOperationResult res, AddyUserInfo* user) {
	std::string retStr;
	switch(res) {
	case AddyDB::kSuccess:
		retStr = user->GetAddress();
		break;

	case AddyDB::kInvalidPin:
		retStr = std::string("The given PIN is invalid!");
		break;

	case AddyDB::kNotFound:
		retStr = std::string("can't find an address entry with the given PIN!");
		break;

	default:
		break;
	}

	mResultPanel->show();
	// set results string
	mpResult->setText(retStr);
}

void AddyPinApp::BuildMainView() {
    /* top container that includes:
     * 1 - new input widget
     * 2 - pin lookup widget
     * 3 - result */
    Wt::WContainerWidget *pTopContainer = new Wt::WContainerWidget(root());
    pTopContainer->setStyleClass("AddyPinMain");

    Wt::WImage *image = new Wt::WImage( Wt::WLink(new Wt::WFileResource("./res/logo.png")), pTopContainer);
    image->setAlternateText("AddyPin logo");
    pTopContainer->addWidget(new WBreak());
    pTopContainer->addWidget(new WBreak());
    pTopContainer->addWidget(new WBreak());
    pTopContainer->addWidget(new WBreak());

    Wt::WAnimation animation(Wt::WAnimation::SlideInFromTop, Wt::WAnimation::EaseOut, 100);

    Wt::WPanel *panel1 = new Wt::WPanel(pTopContainer);
    panel1->setTitle("Register New Address!");
    panel1->setCollapsible(true);
    panel1->setAnimation(animation);

    Wt::WPanel *panel2 = new Wt::WPanel(pTopContainer);
	panel2->setTitle("Lookup an address!");
	panel2->setCollapsible(true);
    panel2->setAnimation(animation);

    Wt::WPanel *panel3 = new Wt::WPanel(pTopContainer);
	panel3->setTitle("Manage your addresses!");
	panel3->setCollapsible(true);
    panel3->setAnimation(animation);

    // new input widget
    Wt::WContainerWidget* pNewInputContainer = new Wt::WContainerWidget(pTopContainer);
    panel1->setCentralWidget(pNewInputContainer);

	Wt::WTable* pTable = new Wt::WTable(pNewInputContainer);
	pTable->setStyleClass("table table-condensed");
	pTable->elementAt(0, 0)->setStyleClass("label");
	pTable->elementAt(0, 0)->addWidget(new WText("Detailed address:"));

	mpInputAddress = new WLineEdit(pNewInputContainer);
	mpInputAddress->setMinimumSize(350, 20);
	mpInputAddress->setFocus();
	pTable->elementAt(0, 1)->addWidget(mpInputAddress);
	pTable->elementAt(0, 1)->setStyleClass("info");
	pTable->elementAt(1, 0)->addWidget(new WText("email:"));
	pTable->elementAt(1, 0)->setStyleClass("label");
	mpInputEmail1 = new WLineEdit(pNewInputContainer);
	mpInputEmail1->setMinimumSize(350, 20);
	pTable->elementAt(1, 1)->addWidget(mpInputEmail1);
	pTable->elementAt(1, 1)->setStyleClass("info");
	mpSubmitButton = new WPushButton("Submit!", pNewInputContainer);
	mpSubmitButton->clicked().connect(this, &AddyPinApp::SubmitNewAddress);
	////////////////////////////////////////////////////////////////
	// lookup widget
	panel2->collapse();
	Wt::WContainerWidget* pLookupContainer = new Wt::WContainerWidget(pTopContainer);
	panel2->setCentralWidget(pLookupContainer);
	Wt::WTable* pTable2 = new Wt::WTable(pLookupContainer);
	pTable2->setStyleClass("table table-condensed");

	pTable2->elementAt(0, 0)->addWidget(new WText("Address Lookup:"));
	pTable2->elementAt(0, 0)->setStyleClass("label");

	mpInputPin = new WLineEdit(pLookupContainer);
	mpInputPin->setMinimumSize(350, 20);
	pTable2->elementAt(0, 1)->addWidget(mpInputPin);
	pTable2->elementAt(0, 1)->setStyleClass("info");
	mpLookupButton = new WPushButton("Lookup!", pLookupContainer);
	mpLookupButton->clicked().connect(this, &AddyPinApp::Lookup);
	////////////////////////////////////////////////////////////////
	// account management widget
	panel3->collapse();
	Wt::WContainerWidget* pAccountContainer = new Wt::WContainerWidget(pTopContainer);
	panel3->setCentralWidget(pAccountContainer);
	mpAccountMngmnt = new AddyAccountMngmnt(pAccountContainer, mrDB);

	//////////////////////////////////////////////////////////////
	pTopContainer->addWidget(new WBreak());
	Wt::WContainerWidget *pResultContainer = new Wt::WContainerWidget(pTopContainer);
	mResultPanel = new Wt::WPanel(pTopContainer);
	mResultPanel->setTitle("Address Assigned!");
	mResultPanel->setCentralWidget(pResultContainer);
	mResultPanel->setCollapsible(true);
	mpResult = new WText(pResultContainer);
	mResultPanel->hide();

}
/*!
 * take the user given name and address and add them to the data base and assign a new AddyPin to the entry
 */
void AddyPinApp::Assign(std::string address, std::string email) {
	std::string pin;
	std::string masterPin;
	AddyDB::EOperationResult res = mrDB.Add(address, email, pin, masterPin);
	std::string plainBody;
	std::string htmlBody;
	switch (res) {
	case AddyDB::kSuccess:
		plainBody = std::string("Thanks for registering another address with us. Your new AddyPin is ");
		plainBody = plainBody.append(pin);
		plainBody = plainBody.append("\nShare it with others using the following link");
		plainBody = plainBody.append(pin).append(".addypin.com\n");
		plainBody = plainBody.append("Thank you,\nAddypin Admin");

		htmlBody = std::string("<p>Thanks for registering another address with us</p><p>Your new AddyPin is <b>");
		htmlBody = htmlBody.append(pin).append("</b></p>");
		htmlBody = htmlBody.append("<p>Share it with others using the following link </p>");
		htmlBody = htmlBody.append("<p><a href=\"").append(pin).append(".addypin.com\">").append(pin).append(".addypin.com</a></p>");
		htmlBody = htmlBody.append("<p>Thank you,</p><p>Addypin Admin</p>");
		SendEmail("Your AddyPin New Address", "AddyPin", email, plainBody, htmlBody);
		mpResult->setText(std::string("Your new Pin is ").append(pin.c_str()).append(", an email has been sent to ").append(mpInputEmail1->text().narrow()));
		break;

	case AddyDB::kSuccessNewUse:
		plainBody = std::string("Thanks for Joining us with your first address. Your first AddyPin is ");
		plainBody = plainBody.append(pin);
		plainBody = plainBody.append("\nShare it with others using the following link");
		plainBody = plainBody.append(pin).append(".addypin.com\n");
		//plainBody = plainBody.append("your master pin is ").append(masterPin).append(", you will need that to manage your addresses\n");
		plainBody = plainBody.append("Thank you,\nAddypin Admin");

		htmlBody = std::string("<p>Thanks for joining us with you first address</p><p>Your first AddyPin is <b>");
		htmlBody = htmlBody.append(pin).append("</b></p>");
		htmlBody = htmlBody.append("<p>Share it with others using the following link </p>");
		htmlBody = htmlBody.append("<p><a href=\"").append(pin).append(".addypin.com\">").append(pin).append(".addypin.com</a></p>");
		//htmlBody = htmlBody.append("<p>your master pin is ").append(masterPin).append(", you will need that to manage your addresses</p>");
		htmlBody = htmlBody.append("<p>Thank you,</p><p>Addypin Admin</p>");
		SendEmail("Your AddyPin Registration", "AddyPin", email, plainBody, htmlBody);
		mpResult->setText(std::string("Your new Pin is ").append(pin.c_str()).append(", an email has been sent to ").append(mpInputEmail1->text().narrow()));
		break;

	case AddyDB::kExceededLimit:
		plainBody = std::string("Thanks for registering your address(es) with us. Unfortunately, Your last trial to create a new AddyPin has failed!");
		plainBody = plainBody.append("You have exceeded the maximum limit for addresses assigned to the same email address!");
		//plainBody = plainBody.append("visit www.addypin.com and click manage your address tab, your master pin is ").append(masterPin);
		plainBody = plainBody.append("Thank you,\nAddypin Admin");

		htmlBody = std::string("<p>Thanks for registering your address with us</p><p>Unfortunately, Your last trial to create a new AddyPin has failed! </p>");
		htmlBody = htmlBody.append("<p>You have exceeded the maximum limit for addresses assigned to the same email address!</p>");
		//htmlBody = htmlBody.append("<p>visit <a href=\"www.addypin.com\"> www.addypin.com</a>  and click manage your address tab, your master pin is <b>").append(masterPin).append("</b></p>");
		htmlBody = htmlBody.append("<p>visit <a href=\"www.addypin.com\"> www.addypin.com</a>  and click manage your address link</p>");
		htmlBody = htmlBody.append("<p>Thank you,</p><p>Addypin Admin</p>");
		SendEmail("Your AddyPins exceeded max limit!", "AddyPin", email, plainBody, htmlBody);
		break;

	default:
		break;
	}
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

	if (!mpInputAddress->text().empty() && !mpInputEmail1->text().empty()) {
		Assign(mpInputAddress->text().narrow(), mpInputEmail1->text().narrow());
	} else {
		mpResult->setText(std::string("can't have empty address or email fields!!"));
	}
	mResultPanel->show();
}

/*!
 * take the user given AddyPin and lookup the corresponding info
 */
void AddyPinApp::Lookup() {
	// validate the AddyPin (only 6 digits) or maybe call a validate function at the AddyPin class
	std::string pin = mpInputPin->text().narrow();
	AddyUserInfo* pRetInfo;
	AddyDB::EOperationResult ret = mrDB.FindByPin(pin, pRetInfo);
	BuildLookupView(ret, pRetInfo);
}
