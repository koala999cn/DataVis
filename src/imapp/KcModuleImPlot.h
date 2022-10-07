#pragma once
#include "KvModule.h"


class KcModuleImPlot : public KvModule
{
public:

	KcModuleImPlot();

	bool initialize() override;

	void deinitialize() override;
};
