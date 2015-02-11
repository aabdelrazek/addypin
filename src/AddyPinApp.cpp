/*
 * AddyPinApp.cpp
 *
 *  Created on: 2015-02-01
 *      Author: aabdelrazek
 */

#include "inc/AddyPinApp.h"
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
AddyPinApp::AddyPinApp(const WEnvironment& env, AddyDB& rDB, AppStartupView view, AddyDB::ELookupResult res, AddyUserInfo* user):
		WApplication(env),
		mStartupView(view),
		mrDB(rDB),
		mpInputAddress(NULL),
		mpNameEdit(NULL),
		mpResult(NULL),
		mpSubmitButton(NULL),
		mpLookupButton(NULL),
		mResultPanel(NULL) {

	setTitle("AddyPin!!");
    useStyleSheet("./res/CSSexample.css");
    setTheme(new WBootstrapTheme());

    switch(mStartupView) {
    case kMainview:
    	BuildMainView();
    	break;

    case kLookupView:
    	Wt::WContainerWidget *pTopContainer = new Wt::WContainerWidget(root());
    	pTopContainer->setStyleClass("AddyPinMain");
    	mResultPanel = new Wt::WPanel(root());
    	mResultPanel->setTitle("Address Lookup Result!");
    	mResultPanel->setCentralWidget(pTopContainer);
    	mpResult = new WText(pTopContainer);
    	BuildLookupView(res, user);
    	break;
    }
}

void AddyPinApp::BuildLookupView(AddyDB::ELookupResult res, AddyUserInfo* user) {
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

    Wt::WPanel *panel1 = new Wt::WPanel(pTopContainer);
    panel1->setTitle("Register New Address!");
    //panel1->addStyleClass("AddyPinMain");
    panel1->setCollapsible(true);
    //panel1->collapseIcon()->()

    Wt::WPanel *panel2 = new Wt::WPanel(pTopContainer);
	panel2->setTitle("Lookup an address!");
	//panel2->addStyleClass("centered-example");
	panel2->setCollapsible(true);

    Wt::WAnimation animation(Wt::WAnimation::SlideInFromTop, Wt::WAnimation::EaseOut, 100);
    panel1->setAnimation(animation);
    panel2->setAnimation(animation);

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
	mpNameEdit = new WLineEdit(pNewInputContainer);
	mpNameEdit->setMinimumSize(350, 20);
	pTable->elementAt(1, 1)->addWidget(mpNameEdit);
	pTable->elementAt(1, 1)->setStyleClass("info");
	mpSubmitButton = new WPushButton("Submit!", pNewInputContainer);
	////////////////////////////////////////////////////////////////
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
	//////////////////////////////////////////////////////////////
	//pTable->columnAt(0)->setStyleClass("col1");
	//pTable2->columnAt(0)->setStyleClass("col1");
	pTopContainer->addWidget(new WBreak());
	Wt::WContainerWidget *pResultContainer = new Wt::WContainerWidget(pTopContainer);
	mResultPanel = new Wt::WPanel(pTopContainer);
	mResultPanel->setTitle("Address Assigned!");
	mResultPanel->setCentralWidget(pResultContainer);
	mResultPanel->setCollapsible(true);
	mpResult = new WText(pResultContainer);
	mResultPanel->hide();

	mpSubmitButton->clicked().connect(this, &AddyPinApp::Submit);
	mpLookupButton->clicked().connect(this, &AddyPinApp::Lookup);


}
/*!
 * take the user given name and address and add them to the data base and assign a new AddyPin to the entry
 */
std::string AddyPinApp::Assign(std::string address, std::string email) {
	AddyUserInfo& rUserInfo = mrDB.CreateUserInfo(address, email);
	std::string pin = mrDB.Add(rUserInfo);

	std::string plainBody = std::string("Thanks for registering your address with us. Your AddyPin is ");
	plainBody = plainBody.append(pin);
	plainBody = plainBody.append("Share it with others using the following link");
	plainBody = plainBody.append("Your <a href=\"").append(pin).append(".addypin.com\"> ").append(pin).append(".addypin.com\n");
	plainBody = plainBody.append("Thank you,\nAddypin Admin");

	std::string htmlBody = std::string("<p>Thanks for registering your address with us</p><p>Your AddyPin is <b>");
	htmlBody = htmlBody.append(pin).append("</b></p>");
	htmlBody = htmlBody.append("<p>Share it with others using the following link </p>");
	htmlBody = htmlBody.append("<p><a href=\"").append(pin).append(".addypin.com\">").append(pin).append(".addypin.com</a></p>");
	htmlBody = htmlBody.append("<p>Thank you,</p><p>Addypin Admin</p>");

	SendEmail("Your AddyPin Registration", "Your AddyPin", email, plainBody, htmlBody);

	return pin;
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

void AddyPinApp::Submit() {
	if (!mpInputAddress->text().empty() && !mpNameEdit->text().empty()) {
		std::string pin = Assign(mpInputAddress->text().narrow(), mpNameEdit->text().narrow());
		mpResult->setText(std::string("Your new Pin is ").append(pin.c_str()).append(", an email has been sent to ").append(mpNameEdit->text().narrow()));
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
	AddyUserInfo retInfo("", "");
	AddyDB::ELookupResult ret = mrDB.FindByPin(pin, retInfo);
	BuildLookupView(ret, &retInfo);
}
