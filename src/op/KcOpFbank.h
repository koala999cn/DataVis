#pragma once
#include "KvOpSampled1dHelper.h"
#include <memory>

class KgFbank;

class KcOpFbank : public KvOpSampled1dHelper
{
	using super_ = KvOpSampled1dHelper;

public:
	KcOpFbank();

	kRange range(kIndex outPort, kIndex axis) const final;

	kReal step(kIndex outPort, kIndex axis) const final;

	bool onNewLink(KcPortNode* from, KcPortNode* to) override;

	bool onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins) final;

	void onStopPipeline() final;

	void showPropertySet() final;

private:

	kIndex osize_(kIndex is) const final;

	bool prepareOutput_() final;

	void op_(const kReal* in, unsigned len, kReal* out) final;

private:
	std::unique_ptr<KgFbank> fbank_;
	int type_;
	int bins_;
	bool normalize_;
	float low_, high_;
};

