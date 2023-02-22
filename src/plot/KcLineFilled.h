#pragma once
#include "KvPlottable.h"
#include <functional>


class KcLineFilled : public KvPlottable
{
	using super_ = KvPlottable;

public:

	using super_::super_;

	const color4f& minorColor() const override;

	void setMinorColor_(const color4f& minor) override;

	unsigned objectCount() const final { return 2; } // fill和edge分别有1个渲染对象

	const KpBrush& fillBrush() const { return fillCxt_; }
	KpBrush& fillBrush() { return fillCxt_; }

	const KpPen& linePen() const { return lineCxt_; }
	KpPen& linePen() { return lineCxt_; }

	bool showLine() const { return showLine_; }
	bool& showLine() { return showLine_; }

	// 面积图的绘制模式
	enum KeFillMode
	{
		k_fill_overlay,
		k_fill_stacked,
		k_fill_between,
		k_fill_dual
	};

	int fillMode() const { return fillMode_; }
	void setFillMode(KeFillMode mode);

private:

	bool objectVisible_(unsigned objIdx) const override;

	void setObjectState_(KvPaint*, unsigned objIdx) const final;

	void* drawObject_(KvPaint*, unsigned objIdx) const final;

	bool objectReusable_(unsigned objIdx) const final;

	using GETTER = std::function<std::vector<float_t>(unsigned ix)>;
	void fillBetween_(KvPaint*, GETTER, GETTER, unsigned count, unsigned ch, void* vtx) const;

private:
	bool showLine_{ false };
	KpPen lineCxt_;
	KpBrush fillCxt_;
	int fillMode_{ 0 };
};