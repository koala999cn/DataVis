#pragma once
#include "KvDataOperator.h"
#include "KgSpectrum.h"


class KcOpSpectrum : public KvDataOperator
{
public:

	KcOpSpectrum();

	bool isStream() const override;

	//kRange range(kIndex axis) const override;

	bool onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins) override;

	void output() override;

private:
	std::unique_ptr<KgSpectrum> spec_;
};

