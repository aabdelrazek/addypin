#ifndef _ADDYTEXT_H_
#define _ADDYTEXT_H_

#define Res(x,y) static const char x[] = y;

Res(kIdNullPlaceholder, "")
Res(kIdFailed,          "Failed!")
Res(kIdWarning,         "Warning!")
Res(kIdError,           "Error!")
Res(kIdSuccess,          "Success!")

Res(kIdInvalidPin,      "The given PIN is invalid!")
Res(kIdEntryNotFound, 	"can't find an address entry with the given PIN!")

Res(kIdRegNewAddr,   	"Register New Address")
Res(kIdDetailedAddr,	"Detailed address:")
Res(kIdSubmit, 			"Submit!")
Res(kIdLookupAddr, 		"Lookup an Address")
Res(kIdAddyToLookup, 	"AddyPin to lookup:")
Res(kIdLookup, 			"Lookup!")
Res(kIdAskForEmail, 	"Provide you email bellow, and we will send you a link that you can use to manage your registered addresses")
Res(kIdSendLink, 		"Send Link!")

Res(kIdMaxLimit,		"Your AddyPins exceeded max limit!")
Res(kIdThankUPlain,		"Thanks for registering another address with us. Your new AddyPin is ")
Res(kIdThankUHtml,		"<p>Thanks for registering another address with us</p><p>Your new AddyPin is <b>")
Res(kIdFrstThankUPlain,	"Thanks for Joining us with your first address. Your first AddyPin is ")
Res(kIdFrstThankUHtml,	"<p>Thanks for joining us with you first address</p><p>Your first AddyPin is <b>")

Res(kIdShareItPlain,	"\nShare it with others using the following link")
Res(kIdShareItHtml,		"<p>Share it with others using the following link </p>")
Res(kIdYourNewPin, 		"Your AddyPin New Address")
Res(kIdEndOfMailPlain, 	"Thank you,\nAddypin Admin")
Res(kIdEndOfMailHtml, 	"<p>Thank you,</p><p>Addypin Admin</p>")

Res(kIdYourNewPinIs,	"Your new Pin is ")
Res(kIdAnEmailSent, 	", an email has been sent to ")
Res(kIdAddr,			"Address")
Res(kIdDelete,			"Delete")
Res(kIdAddyPin,			"AddyPin")
Res(kIdMngAddresses,	"Manage My Addresses!")
Res(kIdSaveChanges,		"Save Changes")
Res(kIdYourAddresses,	"Your Addresses:")
Res(kIdEmail,			"email:")

Res(kIdReqReceivedPlain,"We have received your request to manage your AddyPin addresses!\n")
Res(kIdSecurityPlain,	"For security reasons, the following link is valid for 1 time use and will expire in 10 minutes\n")
Res(kIdWillSendYouPlain,"You will have to ask for another link from our website if you need to manage your addresses later\n")

Res(kIdYourMngmntLink, 	"Your AddyPin Account Management Link")

Res(kIdEmailSentMsg,	"An email has been sent to you with a link to edit/manage your addresses!")
Res(kIdEmailLinkNotMatch,"The link and email provided don't match!!")
Res(kIdEmailNotExist, 	"The provided email does not exist in our system!")
Res(kIdEmptyEmail, 		"type your email first!")
Res(kIdInvalidEmail,	"The email provided is not a valid email address!")
Res(kIdEmptyField,		"can't have empty address or email fields!!")

#endif
