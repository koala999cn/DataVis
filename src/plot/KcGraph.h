#pragma once
#include "KvPlottable1d.h"
#include "KpContext.h"

// 折线图

class KcGraph : public KvPlottable1d
{
	using super_ = KvPlottable1d;

public:

	using super_::super_;

	const color4f& minorColor() const override;

	void setMinorColor_(const color4f& minor) override;

	unsigned objectCount() const override;

	const KpPen& linePen() const { return lineCxt_; }
	KpPen& linePen() { return lineCxt_; }

protected:

	bool objectVisible_(unsigned objIdx) const override;

	void setObjectState_(KvPaint*, unsigned objIdx) const override;

	void* drawObject_(KvPaint*, unsigned objIdx) const override;
	
	// 返回通道ch的线段集合，各线段之间以nan隔开
	KuDataUtil::KpPointGetter1d linesAtChannel_(unsigned ch) const;

private:
	KpPen lineCxt_;
	color4f clrMinor_{ 0, 0, 0, 1 }; // 辅色，用于绘制渐变线条
};
