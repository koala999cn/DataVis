#pragma once
#include "KvModule.h"


class KcModuleImFileDialog : public KvModule
{
public:

	KcModuleImFileDialog();

	bool initialize() override;

	void deinitialize() override;
};
