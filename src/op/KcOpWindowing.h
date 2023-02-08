#pragma once
#include "KvOpSampled1dHelper.h"

class KgWindowing;

class KcOpWindowing : public KvOpSampled1dHelper
{
	using super_ = KvOpSampled1dHelper;

public:
	KcOpWindowing();

	bool onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins) final;

	void onStopPipeline() final;

	void showPropertySet() final;

private:

	void op_(const kReal* in, unsigned len, kReal* out) final;

private:
	std::unique_ptr<KgWindowing> win_;
	int type_;
	float arg_;
};

