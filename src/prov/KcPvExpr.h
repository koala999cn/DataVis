#pragma once
#include "KcPvData.h"


class KcPvExpr : public KcPvData
{
public:
	KcPvExpr();

	void showProperySet() override;

private:
	std::string exprText_;
};