#pragma once
#include "KvRdPlot.h"


class KcRdPlot2d : public KvRdPlot
{
public:

	using super_ = KvRdPlot;

	KcRdPlot2d();

	bool permitInput(int dataSpec, unsigned inPort) const override;

private:

	std::vector<KvPlottable*> createPlottable_(KcPortNode* port) final;

	unsigned supportPlottableTypes_() const final;

	int plottableType_(KvPlottable* plt) const final;

	const char* plottableTypeStr_(int iType) const final;

	KvPlottable* newPlottable_(int iType, const std::string& name) final;

	void showPlottableSpecificProperty_(KvPlottable*) final;
};

