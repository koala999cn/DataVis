#pragma once
#include "KvRdPlot.h"


class KcRdPlot3d : public KvRdPlot
{
public:

	using super_ = KvRdPlot;

	KcRdPlot3d();

	void showPropertySet() final;

private:

	std::vector<KvPlottable*> createPlottable_(KcPortNode* port) final;

	unsigned supportPlottableTypes_() const final;

	int plottableType_(KvPlottable* plt) const final;

	const char* plottableTypeStr_(int iType) const final;

	KvPlottable* newPlottable_(int iType, const std::string& name) final;

	bool plottableMatchData_(int iType, const KvData& d) const final;

	void showPlottableSpecificProperty_(unsigned idx) final;
};

