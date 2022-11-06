#pragma once
#include "KvPlottable1d.h"
#include "KpContext.h"


// 2ά��״ͼ

class KcBars2d : public KvPlottable1d
{
	using super_ = KvPlottable1d;
	using super_::point_getter;

public:

	using super_::super_;

	// bar�Ŀ�����ã�ȡֵ[0, 1]
	// ��ȡ1����bars֮���޿�϶����ȡ0.5����bar�Ŀ����bars֮��Ŀ�϶��ͬ��
	float_t barWidthRatio() const { return barWidthRatio_; }
	float_t& barWidthRatio() { return barWidthRatio_; }

	aabb_type boundingBox() const;


	unsigned majorColorsNeeded() const override;

	bool minorColorNeeded() const override;

	unsigned majorColors() const override;

	const color4f& majorColor(unsigned idx) const override;
	color4f& majorColor(unsigned idx) override;

	void setMajorColors(const std::vector<color4f>& majors) override;

	const color4f& minorColor() const override;
	color4f& minorColor() override;

	void setMinorColor(const color4f& minor) override;

protected:

	void drawImpl_(KvPaint*, point_getter, unsigned, unsigned) const override;

	// ���㵥��bar�Ŀ�ȣ��������꣩
	float_t barWidth_() const;

protected:
	KpPen border_;
	KpBrush fill_;
	float_t barWidthRatio_{ 0.5 };
	float_t baseLine_{ 0 }; // bar�ĵ���
};
