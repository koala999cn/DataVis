#pragma once
#include "KvRdPlot.h"


class KcRdPlot1d : public KvRdPlot
{
public:

	using super_ = KvRdPlot;

	KcRdPlot1d();

private:

	std::vector<KvPlottable*> createPlottable_(KvDataProvider* prov) final;

	void showPlottableProperty_(unsigned idx) final;

	static int plottableType_(KvPlottable* plt);
	static KvPlottable* createPlottable_(int type, const std::string& name);
};

