#pragma once
#include "KvRenderable.h"
#include <string>
#include <memory>
#include "KvData.h"
#include "KtColor.h"


class KvPlottable : public KvRenderable
{
public:
	using data_ptr = std::shared_ptr<KvData>;

	using KvRenderable::KvRenderable;

	data_ptr data() const { return data_; }
	data_ptr& data() { return data_; }

	bool empty() const {
		return !data_ || data_->size() == 0;
	}

	aabb_type boundingBox() const override;

	// ����-1��ʾ��Ҫ����ɫ��
	virtual unsigned majorColorsNeeded() const = 0;

	// ����false��ʾ����Ҫ����ɫ
	virtual bool minorColorNeeded() const = 0;

	// ��ɫ�ʵĸ�����һ�����majorColorNeeded��
	// ������ɫ������£�����������ɫ�ĸ���
	virtual unsigned majorColors() const {
		return majorColorsNeeded();
	}

	virtual const color4f& majorColor(unsigned idx) const = 0;
	virtual color4f& majorColor(unsigned idx) = 0;

	virtual const color4f & minorColor() const = 0;
	virtual color4f& minorColor() = 0;

private:
	data_ptr data_;
};
