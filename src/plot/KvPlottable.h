#pragma once
#include "KvRenderable.h"
#include <string>
#include <memory>
#include "KtColor.h"

class KvData;
class KvDiscreted;

class KvPlottable : public KvRenderable
{
public:
	using data_ptr = std::shared_ptr<KvData>;
	using point3 = KtPoint<float_t, 3>;

	using KvRenderable::KvRenderable;

	data_ptr data() const { return data_; }
	void setData(data_ptr d);

	bool empty() const;

	unsigned sampCount(unsigned dim) const { return sampCount_[dim]; }
	unsigned& sampCount(unsigned dim) { return sampCount_[dim]; }

	aabb_type boundingBox() const override;

	// ��װ�������ݵĻ��ƣ��ṩ����һ��������ɢ���ݵĽӿ�drawDiscreted_
	void draw(KvPaint*) const override;


	/// ����Ϊ��ɫ��ͨ�ýӿ� //////////////////////////////////

	// ����-1��ʾ��Ҫ����ɫ��
	virtual unsigned majorColorsNeeded() const = 0;

	// ����false��ʾ����Ҫ����ɫ
	virtual bool minorColorNeeded() const = 0;

	// ��ɫ�ʵĸ�����һ�����majorColorNeeded��
	// ������ɫ������£�����������ɫ�ĸ���
	virtual unsigned majorColors() const = 0;

	virtual color4f majorColor(unsigned idx) const = 0;

	virtual void setMajorColors(const std::vector<color4f>& majors) = 0;

	virtual color4f minorColor() const = 0;

	virtual void setMinorColor(const color4f& minor) = 0;

	////////////////////////////////////////////////////////////////

private:

	virtual void drawDiscreted_(KvPaint*, KvDiscreted*) const = 0;

private:

	data_ptr data_;

	// ��ά�ȵĲ�������Ŀ, ����������������
	std::vector<unsigned> sampCount_{ std::vector<unsigned>({ 1000 }) }; 
};
