#pragma once
#include "KvContinued.h"
#include <memory>


// 提取单通道连续数据

class KcMonoContinued : public KvContinued
{
	using data_ptr = std::shared_ptr<KvContinued>;

public:

	// 提取数据d的第ch通道数据
	KcMonoContinued(data_ptr d, kIndex ch) : d_(d), channel_(ch) {
		assert(ch >= 0 && ch < d->channels());
		ranges_.resize(dim());
		for (unsigned i = 0; i < dim(); i++)
			ranges_[i] = d_->range(i);
	}

	// 实现KvData的接口

	kIndex dim() const override {
		return d_->dim();
	}

	kIndex channels() const override {
		return 1;  // 始终单通道
	}

	kRange range(kIndex axis) const override {
		return axis == dim() ? d_->range(axis) // TODO: 通道值域range没有缓存，会很慢 valueRange(0) 
			: ranges_[axis];
	}

	kRange valueRange(kIndex channel) const override {
		assert(channel == 0);
		return d_->valueRange(channel_);
	}


	// 实现KvContinued的接口

	kReal value(kReal pt[], kIndex channel) const override {
		assert(channel == 0);
		return d_->value(pt, channel_);
	}

	void setRange(kIndex axis, kReal low, kReal high) override {
		ranges_[axis] = { low, high };
	}

private:
	data_ptr d_;
	kIndex channel_;
	std::vector<kRange> ranges_;
};
