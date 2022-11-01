#pragma once
#include "KvDataOperator.h"
#include "KgSpectrum.h"


class KcOpSpectrum : public KvDataOperator
{
	using super_ = KvDataOperator;

public:

	KcOpSpectrum();

	int spec(kIndex outPort) const final;

	kRange range(kIndex outPort, kIndex axis) const final;

	bool onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins) final;

	void output() final;

	void showProperySet() final;

	bool permitInput(int dataSpec, unsigned inPort) const final;

private:
	std::unique_ptr<KgSpectrum> spec_;
	int specType_{ 0 }; // 谱类型
	int normMode_{ 0 }; // 谱的规范化方式
	bool roundToPower2_{ false }; // 是否对输入进行round
};

