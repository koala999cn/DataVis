#pragma once
#include "KvDataOperator.h"
#include <vector>

class KcSampler;

// 采样器实现：将连续数据和数组数据转换为采样数据
// 可用于将连续数据、序列数据转换为采样数据，或者变更采样数据的采样率（与重采样不同，此处会根据step变化同比列缩放range）
// 对连续数据，保持range恒定、变更size、推导step
// 对离散数据，保持size恒定、变更step、推导range. 
class KcOpSampler : public KvDataOperator
{
	using super_ = KvDataOperator;

public:
	KcOpSampler();

	int spec(kIndex outPort) const final;

	kRange range(kIndex outPort, kIndex axis) const final;

	kReal step(kIndex outPort, kIndex axis) const final;

	kIndex size(kIndex outPort, kIndex axis) const final;

	bool onNewLink(KcPortNode* from, KcPortNode* to) final;

	void onDelLink(KcPortNode* from, KcPortNode* to) final;

	void showPropertySet() final;

	bool permitInput(int dataSpec, unsigned inPort) const final;

	
private:

	bool prepareOutput_() final;

	void outputImpl_() final;

	void syncInput_(); // 根据输入同步输出对象，调用前确保idata_已是最新

	void syncOutput_(); // 根据sampCount_同步输出对象的参数

private:
	std::vector<float> sampCount_; // 对连续数据表示sampCount，对离散数据表示sampRate
};

