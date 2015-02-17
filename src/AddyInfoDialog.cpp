#include "inc/AddyInfoDialog.h"
#include <functional>

AddyInfoDialog::AddyInfoDialog(std::string title, std::string textMsg) {
	mpMessageBox = new Wt::WMessageBox (title, textMsg, Wt::Information, Wt::Ok);
	mpMessageBox->setModal(true);

	mpMessageBox->buttonClicked().connect(std::bind( [=] () { delete mpMessageBox; } ) );
	mpMessageBox->show();
}

