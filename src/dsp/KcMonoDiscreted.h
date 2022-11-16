#pragma once
#include "KvDiscreted.h"
#include <memory>


// 提取单通道离散数据

class KcMonoDiscreted : public KvDiscreted
{
	using data_ptr = std::shared_ptr<KvDiscreted>;

public:

	// 提取数据d的第ch通道数据
	KcMonoDiscreted(data_ptr d, kIndex ch) : d_(d), channel_(ch) {
		assert(ch >= 0 && ch < d->channels());
	}

	// 实现KvData的接口

	kIndex dim() const override {
		return d_->dim();
	}

	kIndex size() const override {
		return d_->size();
	}

	kIndex channels() const override {
		return 1;  // 始终单通道
	}

	kRange range(kIndex axis) const override {
		if (axis == dim())
			return valueRange(0);

		return d_->range(axis);
	}

	kRange valueRange(kIndex channel) const override {
		assert(channel == 0);
		return d_->valueRange(channel_);
	}


	// 实现KvDiscreted的接口

	void resize(kIndex shape[], kIndex channels = 0) override {
		assert(false);
	}

	kIndex size(kIndex axis) const override {
		return d_->size(axis);
	}

	kReal step(kIndex axis) const override {
		return d_->step(axis);
	}

	void clear() override {
		assert(false);
	}

	kReal valueAt(kIndex n, kIndex channel) const override {
		assert(channel == 0);
		return d_->valueAt(n, channel_);
	}

	std::vector<kReal> pointAt(kIndex n, kIndex channel) const override {
		assert(channel == 0);
		return d_->pointAt(n, channel_);
	}

	kReal xToIndex(kReal x) const override {
		return d_->xToIndex(x);
	}

private:
	data_ptr d_;
	kIndex channel_;
};