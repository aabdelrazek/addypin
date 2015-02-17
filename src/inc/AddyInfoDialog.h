#include <Wt/WContainerWidget>
#include <Wt/WMessageBox>
#include <Wt/WPushButton>
#include <Wt/WText>

class AddyInfoDialog {
public:
	AddyInfoDialog(std::string title, std::string textMsg);

private:
	Wt::WMessageBox *mpMessageBox;
};
