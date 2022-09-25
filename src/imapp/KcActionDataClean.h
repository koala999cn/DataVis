#pragma once
#include "KvAction.h"


class KcActionDataClean : public KvAction
{
public:
	KcActionDataClean();

	bool trigger() override;

	void update() override;
};