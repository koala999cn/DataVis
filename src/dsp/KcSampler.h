#pragma once
#include <memory>
#include <vector>
#include "KvSampled.h"


// 对连续数据的采样器

class KcSampler : public KvSampled
{
public:
	using super_ = KvSampled;
	
	KcSampler(std::shared_ptr<KvData> data);

	/// KvData接口

	kIndex dim() const final;

	kIndex channels() const final;

	kRange range(kIndex axis) const final;

	using KvSampled::size;

	/// KvDiscreted接口

	void resize(kIndex shape[], kIndex channels = 0) final;

	kIndex size(kIndex axis) const final;

	kReal step(kIndex axis) const final;

	void clear() final;

	kReal xToIndex(kReal x) const final;


	/// KvSampled接口

	void reset(kIndex axis, kReal low, kReal step, kReal x0_ref = 0) final;

	kReal value(kIndex idx[], kIndex channel) const final;

	std::vector<kReal> point(kIndex idx[], kIndex channel) const final;

private:
	const std::shared_ptr<KvData> internal_;
	std::vector<kReal> steps_;
	std::vector<kReal> x0refs_;
};
