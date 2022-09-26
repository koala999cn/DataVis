#include "KcActionLoadText.h"
#include "KcActionFileDialog.h"
#include "KcActionDataClean.h"


KcActionLoadText::KcActionLoadText()
	: KcActionSequence("Text Data", "insert a provider node loading data from text file")
{
	auto fileDialog = std::make_shared<KcActionFileDialog>(
		KcActionFileDialog::KeType::k_open,
		"Select text data file",
		"text file (*.txt;*.cvs){.txt,.cvs},.*");

	auto dataCleam = std::make_shared<KcActionDataClean>(
		fileDialog->result()
		);

	addAction(fileDialog);
	addAction(dataCleam);
}
