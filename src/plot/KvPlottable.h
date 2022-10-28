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

	// ����-1��ʾ��Ҫ����ɫ��
	virtual unsigned majorColorsNeeded() const { 
		return !shareColor() && data() ? data()->channels() : 1; 
	}

	// ����false��ʾ����Ҫ����ɫ
	virtual bool minorColorNeeded() const { return 0; }

	// ��ɫ�ʵĸ�����һ�����majorColorNeeded��
	// ������ɫ������£�����������ɫ�ĸ���
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
	bool shareColor_{ false }; // ��ͨ�������Ƿ��õ�ɫ��

	std::vector<color4f> majorColors_{ color4f(0, 0, 0, 1) };
	color4f minorColor_;

	// ��ά�ȵĲ�������Ŀ, ����������������
	std::vector<unsigned> sampCount_{ std::vector<unsigned>({ 1000 }) }; 
};
