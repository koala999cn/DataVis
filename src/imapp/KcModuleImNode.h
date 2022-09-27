#pragma once
#include "KvModule.h"


class KcModuleImNode : public KvModule
{
public:

	KcModuleImNode();

	bool initialize() override;

	void deinitialize() override;
};
