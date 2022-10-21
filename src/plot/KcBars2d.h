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

	float barWidthRatio() const { return barWidthRatio_; }
	float& barWidthRatio() { return barWidthRatio_; }

protected:

	void drawImpl_(KvPaint*, point_getter, const color4f&) const override;

	// ���ص���bar�Ŀ�ȣ��������꣩
	float barWidth_() const; 

protected:
	KpPen border_;
	KpBrush fill_;
	float barWidthRatio_{ 0.5 };
};
