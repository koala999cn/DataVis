#pragma once
#include "KvOpSampled1dHelper.h"
#include <memory>


class KgHistC;

class KcOpHistC : public KvOpSampled1dHelper
{
	using super_ = KvOpSampled1dHelper;

public:
	KcOpHistC();

	int spec(kIndex outPort) const final;

	kRange range(kIndex outPort, kIndex axis) const final;

	kReal step(kIndex outPort, kIndex axis) const final;

	kIndex size(kIndex outPort, kIndex axis) const final;

	bool permitInput(int dataSpec, unsigned inPort) const final;

	bool onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins) final;

	void onStopPipeline() final;

	void showPropertySet() final;

	// 根据输入同步low_ & high_
	bool onNewLink(KcPortNode* from, KcPortNode* to) final;

private:

	kIndex isize_() const final { return 0; } // 支持任意长度的输入

	kIndex osize_(kIndex is) const final;

	// 重载该函数，以支持动态设定参数
	bool prepareOutput_() final;

	void op_(const kReal* in, unsigned len, unsigned ch, kReal* out) final;

private:
	std::vector<std::unique_ptr<KgHistC>> histc_; // 支持多通道输入，每个通道1个histc对象
	int bins_;
	float low_, high_;
};

