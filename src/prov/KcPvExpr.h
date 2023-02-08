#pragma once
#include "KcPvData.h"


class KcPvExpr : public KcPvData
{
public:
	KcPvExpr();

	void showPropertySet() override;

private:
	std::string exprText_;
};