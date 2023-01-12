#pragma once
#include "KvRenderable.h"
#include <memory>
#include "KtColor.h"
#include "KcAxis.h"

class KvData;
class KvDiscreted;

// �ɻ��ƶ���ĳ�����
// ����ʵ�ֶ��������ݵĲ�����������ֻ�������ɢ����

class KvPlottable : public KvRenderable
{
public:
	using data_ptr = std::shared_ptr<KvData>;
	using point3 = KtPoint<float_t, 3>;

	using KvRenderable::KvRenderable;

	bool empty() const;

	data_ptr data() const { return data_; }
	void setData(data_ptr d);

	unsigned sampCount(unsigned dim) const { return sampCount_[dim]; }
	unsigned& sampCount(unsigned dim) { return sampCount_[dim]; }

	// ���õ�dimά�ȵķ��������ᣬ�ú����ӹ�axis�Ŀ���Ȩ
	void setAxis(unsigned dim, KcAxis* axis) { selfAxes_[dim].reset(axis); }

	// ����dimά�ȵķ�������������
	KcAxis* axis(unsigned dim) const { return selfAxes_[dim].get(); }

	// ��plt�Ƿ��з���������
	bool hasSelfAxis() const;

	aabb_t boundingBox() const override;

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

	float_t defaultZ() const { return defaultZ_; }
	float_t& defaultZ() { return defaultZ_; }

	float_t stepZ() const { return stepZ_; }
	float_t& stepZ() { return stepZ_; }

private:

	virtual void drawDiscreted_(KvPaint*, KvDiscreted*) const = 0;

private:

	data_ptr data_;

	// ��ά�ȵĲ�������Ŀ, ����������������
	std::vector<unsigned> sampCount_{ std::vector<unsigned>({ 1000 }) }; 

	std::array<std::unique_ptr<KcAxis>, 3> selfAxes_; // ���ڷ��������ᣬȱʡΪnull����ʾʹ����������

	// ���³�Ա���Զ�ά������Ч

	float_t defaultZ_{ 0 }; // ��ά���ݵ�z�Ὣ����Ϊ��ֵ
	float_t stepZ_{ 1 }; // ��ͨ����ά���ݵ�z��ƫ�ơ����뽫��ͨ��������ʾ��һ��zƽ�棬�ø�ֵΪ0
};
