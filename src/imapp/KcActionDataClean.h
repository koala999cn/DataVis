#pragma once
#include "KvAction.h"


class KcActionDataClean : public KvAction
{
public:
	KcActionDataClean(const std::string& filepath);

	bool trigger() override;

	void update() override;

private:
	const std::string& filepath_;
};