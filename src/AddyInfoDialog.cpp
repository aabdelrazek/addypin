#include "inc/AddyInfoDialog.h"
#include <functional>

AddyInfoDialog::AddyInfoDialog() : mMessageBox(NULL) {
	mMessageBox.setModal(false);
	mMessageBox.buttonClicked().connect(std::bind( [=] () { mMessageBox.hide(); } ) );
	mMessageBox.setButtons(Wt::Ok);
	mMessageBox.setIcon(Wt::Information);
}

void AddyInfoDialog::Show(std::string title, std::string textMsg) {
	mMessageBox.setText(textMsg);
	mMessageBox.setWindowTitle(title);
	mMessageBox.show();
}
