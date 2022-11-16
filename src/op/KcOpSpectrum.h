#pragma once
#include "KvOpSampled1dHelper.h"
#include "KgSpectrum.h"


class KcOpSpectrum : public KvOpSampled1dHelper
{
	using super_ = KvOpSampled1dHelper;

public:

	KcOpSpectrum();

	kRange range(kIndex outPort, kIndex axis) const final;

	kReal step(kIndex outPort, kIndex axis) const final;

	bool onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins) final;

	void onStopPipeline() final;

	void showProperySet() final;

private:
	
	kIndex isize_() const final;

	kIndex osize_(kIndex is) const final;

	void op_(const kReal* in, unsigned len, kReal* out) final;

private:
	std::unique_ptr<KgSpectrum> spec_;
	int specType_{ 0 }; // 谱类型
	int normMode_{ 0 }; // 谱的规范化方式
	bool roundToPower2_{ false }; // 是否对输入进行round
};

