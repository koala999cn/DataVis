#pragma once
#include "KvContinued.h"
#include <memory>


// ��ȡ��ͨ����������

class KcMonoContinued : public KvContinued
{
	using data_ptr = std::shared_ptr<KvContinued>;

public:

	// ��ȡ����d�ĵ�chͨ������
	KcMonoContinued(data_ptr d, kIndex ch) : d_(d), channel_(ch) {
		assert(ch >= 0 && ch < d->channels());
		ranges_.resize(dim());
		for (unsigned i = 0; i < dim(); i++)
			ranges_[i] = d_->range(i);
	}

	// ʵ��KvData�Ľӿ�

	kIndex dim() const override {
		return d_->dim();
	}

	kIndex channels() const override {
		return 1;  // ʼ�յ�ͨ��
	}

	kRange range(kIndex axis) const override {
		return axis == dim() ? d_->range(axis) // TODO: ͨ��ֵ��rangeû�л��棬����� valueRange(0) 
			: ranges_[axis];
	}

	kRange valueRange(kIndex channel) const override {
		assert(channel == 0);
		return d_->valueRange(channel_);
	}


	// ʵ��KvContinued�Ľӿ�

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
