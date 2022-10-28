#pragma once
#include "KvRenderable.h"
#include <string>
#include <memory>
#include "KvData.h"
#include "KtColor.h"
#include "KtAABB.h"


class KvPlottable : public KvRenderable
{
public:
	using data_ptr = std::shared_ptr<KvData>;
	using point3 = KtPoint<float_t, 3>;

	using KvRenderable::KvRenderable;

	data_ptr data() const { return data_; }
	void setData(data_ptr d) { 
		data_ = d; 
		majorColors_.resize(d->channels(), color4f{ 0, 0, 0, 1 });

		if (d->isContinued() && d->dim() != sampCount_.size() ) 
			sampCount_.assign(d->dim(), std::pow(1000., 1. / d->dim()));
	}

	bool empty() const {
		return !data_ || data_->size() == 0;
	}

	aabb_type boundingBox() const override;

	// 返回-1表示需要连续色带
	virtual unsigned majorColorsNeeded() const { 
		return !shareColor() && data() ? data()->channels() : 1; 
	}

	// 返回false表示不需要辅助色
	virtual bool minorColorNeeded() const { return 0; }

	// 主色彩的个数，一般等于majorColorNeeded。
	// 在连续色彩情况下，返回主控制色的个数
	virtual unsigned majorColors() const { return majorColors_.size(); }

	virtual const color4f& majorColor(unsigned idx) const {	return majorColors_[idx]; }
	virtual color4f& majorColor(unsigned idx) { return majorColors_[idx]; }

	virtual void setMajorColors(const std::vector<color4f>& majors) { majorColors_ = majors; }

	virtual const color4f& minorColor() const { return minorColor_; }
	virtual color4f& minorColor() { return minorColor_; }

	virtual void setMinorColor(const color4f& minor) { minorColor_ = minor; }

	bool shareColor() const { return shareColor_; }
	bool& shareColor() { return shareColor_; }

	unsigned sampCount(unsigned dim) const {
		return sampCount_[dim];
	}

	unsigned& sampCount(unsigned dim) {
		return sampCount_[dim];
	}

private:

	data_ptr data_;
	bool shareColor_{ false }; // 多通道数据是否共用调色板

	std::vector<color4f> majorColors_{ color4f(0, 0, 0, 1) };
	color4f minorColor_;

	// 各维度的采样点数目, 仅适用于连续数据
	std::vector<unsigned> sampCount_{ std::vector<unsigned>({ 1000 }) }; 
};
