#pragma once
#include "KvRdPlot.h"


class KcRdPlot3d : public KvRdPlot
{
public:

	using super_ = KvRdPlot;

	KcRdPlot3d();

	void showProperySet() final;

private:

	std::vector<KvPlottable*> createPlottable_(KvDataProvider* prov) final;
};

