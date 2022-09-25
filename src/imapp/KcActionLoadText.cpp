#include "KcActionLoadText.h"
#include "KcActionFileDialog.h"


KcActionLoadText::KcActionLoadText()
	: KcActionSequence("Text Data", "insert a data-provider node from text file")
{
	addAction(std::make_shared<KcActionFileDialog>(
		KcActionFileDialog::KeType::k_open,
		"Load Text Data", 
		"text file (*.txt;*.cvs){.txt,.cvs},.*"));
}
