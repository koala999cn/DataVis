#pragma once
#include "KvCustomPlot.h"


class KcPlot2d : public KvCustomPlot
{
public:
	KcPlot2d(KvDataProvider* is);

	bool render(std::shared_ptr<KvData> data) override;

	void reset() override;
};

