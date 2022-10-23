#include "KcActionNewTextData.h"
#include "KcActionShowFileDialog.h"
#include "KcActionTextLoadAndClean.h"
#include "KcActionInsertDataNode.h"


KcActionNewTextData::KcActionNewTextData()
	: KcActionSequence("Text Data", "insert a provider node loading data from text file")
{
	auto fileDialog = std::make_shared<KcActionShowFileDialog>(
		KcActionShowFileDialog::KeType::k_open,
		"Select text data file",
		"text file (*.txt;*.cvs){.txt,.cvs},.*");

	auto dataClean = std::make_shared<KcActionTextLoadAndClean>(
		fileDialog->result()
		);

	auto insertNode = std::make_shared<KcActionInsertDataNode>(
		fileDialog->result(),
		dataClean->cleanData());

	addAction(fileDialog);
	addAction(dataClean);
	addAction(insertNode);
}
