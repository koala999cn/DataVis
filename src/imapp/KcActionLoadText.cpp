#include "KcActionLoadText.h"
#include "KcActionFileDialog.h"


KcActionLoadText::KcActionLoadText()
	: KcActionSequence("Text Data", "insert a provider node loading data from text file")
{
	addAction(std::make_shared<KcActionFileDialog>(
		KcActionFileDialog::KeType::k_open,
		"Select text data file", 
		"text file (*.txt;*.cvs){.txt,.cvs},.*"));
}
