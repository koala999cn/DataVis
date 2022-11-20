#pragma once
#include "KcActionShowFileDialog.h"


class KcActionNewAudioData : public KcActionShowFileDialog
{
	using super_ = KcActionShowFileDialog;

public:
	KcActionNewAudioData();

	void update() final;
};
