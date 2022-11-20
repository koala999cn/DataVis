#pragma once
#include "KvDataOperator.h"
#include <memory>


class KgHistC;

class KcOpHistC : public KvDataOperator
{
	using super_ = KvDataOperator;

public:
	KcOpHistC();

	int spec(kIndex outPort) const final;

	kRange range(kIndex outPort, kIndex axis) const final;

	kReal step(kIndex outPort, kIndex axis) const final;

	kIndex size(kIndex outPort, kIndex axis) const final;

	bool permitInput(int dataSpec, unsigned inPort) const final;

	bool onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins) final;

	void onStopPipeline() final;

	void output() final;

	void showProperySet() final;

	// 根据输入同步low_ & high_
	bool onNewLink(KcPortNode* from, KcPortNode* to) final;

	// 重载该函数，以支持动态设定参数
	void onNewFrame(int frameIdx) final;

private:
	std::unique_ptr<KgHistC> histc_;
	int bins_;
	float low_, high_;
};

