#pragma once
#include "KvOpSampled1dHelper.h"
#include <vector>

class KgResampler;

class KcOpResampler : public KvOpSampled1dHelper
{
	using super_ = KvOpSampled1dHelper;

public:
	KcOpResampler();

	kReal step(kIndex outPort, kIndex axis) const override;

	bool onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins) final;

	void onStopPipeline() final;

	void onNewFrame(int frameIdx) final;

	void showProperySet() final;


private:

	// KvOpSampled1dHelper接口

	kIndex isize_() const final;

	kIndex osize_(kIndex is) const final;

	void op_(const kReal* in, unsigned len, kReal* out) final;

private:
	std::unique_ptr<KgResampler> resamp_;
	kReal factor_; // 重采样系数
	int wsize_;
	int method_;
};

