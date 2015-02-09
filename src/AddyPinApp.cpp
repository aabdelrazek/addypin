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

/*
 * The env argument contains information about the new session, and
 * the initial request. It must be passed to the WApplication
 * constructor so it is typically also an argument for your custom
 * application constructor.
*/
AddyPinApp::AddyPinApp(const WEnvironment& env, AddyDB& rDB):
		WApplication(env),
		mrDB(rDB),
		mpInputAddress(NULL),
		mpNameEdit(NULL),
		mpResult(NULL),
		mpSubmitButton(NULL),
		mpLookupButton(NULL) {
  setTitle("AddyPin!!");

  /* top container that includes:
   * 1 - new input widget
   * 2 - pin lookup widget
   * 3 - result
   */
  Wt::WContainerWidget *pTopContainer = new Wt::WContainerWidget(root());

  // new input widget
  Wt::WContainerWidget* pNewInputContainer = new Wt::WContainerWidget(pTopContainer);
  //Wt::WCssDecorationStyle style;
  //Wt::WBorder brdr(WBorder::Dashed);
  //style.setBorder(brdr);
  //pNewInputContainer->setDecorationStyle(style);

  Wt::WContainerWidget* pAddr = new Wt::WContainerWidget(pNewInputContainer);
  pAddr->addWidget(new WText("Detailed address:"));
  mpInputAddress = new WLineEdit(pAddr);
  mpInputAddress->setMinimumSize(450, 20);
  mpInputAddress->setFocus();
  Wt::WContainerWidget* pEmail = new Wt::WContainerWidget(pNewInputContainer);
  pEmail->addWidget(new WText("email:"));
  mpNameEdit = new WLineEdit(pEmail);
  mpNameEdit->setMinimumSize(150, 20);
  mpSubmitButton = new WPushButton("Submit!", pNewInputContainer);
  mpSubmitButton->setMargin(5, Left);

  // lookup widget
  Wt::WContainerWidget* pLookupContainer = new Wt::WContainerWidget(pTopContainer);
  pLookupContainer->addWidget(new WText("AddyPin for address lookup:"));
  mpInputPin = new WLineEdit(pLookupContainer);
  mpLookupButton = new WPushButton("Lookup!", pLookupContainer);
  mpLookupButton->setMargin(5, Right);

  pTopContainer->addWidget(new WBreak());
  mpResult = new WText(pTopContainer);

  mpSubmitButton->clicked().connect(this, &AddyPinApp::Assign);
  mpLookupButton->clicked().connect(this, &AddyPinApp::Lookup);

}

/*!
 * take the user given name and address and add them to the data base and assign a new AddyPin to the entry
 */
void AddyPinApp::Assign()
{
	AddyUserInfo& rUserInfo = mrDB.CreateUserInfo(mpInputAddress->text().narrow(), mpNameEdit->text().narrow());
	AddyPin pin = mrDB.Add(rUserInfo);
	mpResult->setText(std::string("your Pin is: ").append(pin.GetPin().c_str()));

	Mail::Message message;
	message.setFrom(Mail::Mailbox("admin@addypin.com", "Your AddyPin"));
	message.addRecipient(Mail::To, Mail::Mailbox(mpNameEdit->text().narrow(), mpNameEdit->text().narrow()));
	message.setSubject("Your AddyPin Registration");

	std::string plainBody = std::string("Thanks for registering your address with us. Your AddyPin is ");
	plainBody = plainBody.append(pin.GetPin());
	plainBody = plainBody.append("\nThank you,\nYour http://www.addypin.com Admin Team");
	message.setBody(WString(plainBody));

	std::string htmlBody = std::string("<p>Thanks for registering your address with us<\p><p>Your AddyPin is <b>");
	htmlBody = htmlBody.append(pin.GetPin()).append("</b></p>");
	htmlBody = htmlBody.append("<p>Thank you</p>");
	htmlBody = htmlBody.append("<p>Your <a href=\"http://www.addypin.com\"> addypin </a> Admin Team</p>");
	message.addHtmlBody(WString(htmlBody));

	Mail::Client client;
	client.connect();
	//client.connect("smtp.office365.com", 587);
	client.send(message);
}

/*!
 * take the user given AddyPin and lookup the corresponding info
 */
void AddyPinApp::Lookup()
{
	// validate the AddyPin (only 6 digits) or maybe call a validate function at the AddyPin class
	std::string pin = mpInputPin->text().narrow();
	AddyUserInfo retInfo("", "");
	AddyDB::ELookupResult ret = mrDB.FindByPin(pin, retInfo);
	std::string retStr = "";
	switch(ret) {
	case AddyDB::kSuccess:
		retStr = std::string("the AddyPin \"").append(pin.c_str()).append("\" is for ").append(retInfo.GetUserName().c_str()).append(" , The address is: ").append(retInfo.GetAddress().c_str());
		break;

	case AddyDB::kInvalidPin:
		retStr = std::string("The given PIN is invalid!");
		break;

	case AddyDB::kNotFound:
		retStr = std::string("can't find an address entry with the given PIN!");
		break;
	}
	// set results string
	mpResult->setText(retStr);
}
