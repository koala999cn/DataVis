#include "KcActionLoadText.h"
#include "ImFileDialog/ImFileDialog.h"


auto KcActionLoadText::desc() const -> const std::string_view
{
	return "insert a data-provider node from text file";
}


bool KcActionLoadText::trigger()
{
	if (ifd::FileDialog::Instance().Open(
		"opentext", "Load Text Data", "text file (*.txt;*.cvs){.txt,.cvs},.*")) {
		state_ = KeState::k_triggered;
		return true;
	}

	return false;
}


void KcActionLoadText::update()
{
	if (ifd::FileDialog::Instance().IsDone("opentext")) {
		if (ifd::FileDialog::Instance().HasResult()) {
			state_ = KeState::k_done;
			std::string res = ifd::FileDialog::Instance().GetResult().u8string();
		}
		else {
			state_ = KeState::k_cancelled;
		}

		ifd::FileDialog::Instance().Close();
	}
}
