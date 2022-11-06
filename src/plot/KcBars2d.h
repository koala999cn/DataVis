#pragma once
#include "KvPlottable1d.h"
#include "KpContext.h"


// 2维柱状图

class KcBars2d : public KvPlottable1d
{
	using super_ = KvPlottable1d;
	using super_::point_getter;

public:

	using super_::super_;

	// bar的宽度设置，取值[0, 1]
	// 若取1，则bars之间无空隙；若取0.5，则bar的宽度与bars之间的空隙相同。
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

	// 计算单个bar的宽度（世界坐标）
	float_t barWidth_() const;

protected:
	KpPen border_;
	KpBrush fill_;
	float_t barWidthRatio_{ 0.5 };
	float_t baseLine_{ 0 }; // bar的底线
};
