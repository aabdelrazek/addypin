#ifndef _ADDY_INFO_DIALOG_
#define _ADDY_INFO_DIALOG_

#include <Wt/WContainerWidget>
#include <Wt/WMessageBox>
#include <Wt/WPushButton>
#include <Wt/WText>

class AddyInfoDialog {
public:
	AddyInfoDialog();
	void Show(std::string title, std::string textMsg);
private:
	Wt::WMessageBox mMessageBox;
	AddyInfoDialog(AddyInfoDialog const&) = delete;
    void operator=(AddyInfoDialog const&) = delete;
};

#endif //_ADDY_INFO_DIALOG_
