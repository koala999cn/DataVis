#pragma once
#include "KvRdPlot.h"


class KcRdPlot1d : public KvRdPlot
{
public:

	using super_ = KvRdPlot;

	KcRdPlot1d();

private:

	std::vector<KvPlottable*> createPlottable_(KvDataProvider* prov) final;

	unsigned supportPlottableTypes_() const final;

	int plottableType_(KvPlottable* plt) const final;

	const char* plottableTypeStr_(int iType) const final;

	KvPlottable* newPlottable_(int iType, const std::string& name) final;

};

