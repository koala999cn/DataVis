#pragma once
#include "KvPlottable.h"
#include "KvPaint.h"

class KvSampled;

// image图的基类，主要处理grid（sampled2d）数据，用于绘制热图heatmap、曲面图surface等
// 兼容2d和3d模式

class KvPlottable2d : public KvPlottable
{
	using super_ = KvPlottable;

public:

	using super_::super_;

	const color4f& minorColor() const override;

	void setMinorColor(const color4f& minor) override;

	bool showFill() const { return showFill_; }
	bool& showFill() { return showFill_; }

	bool showBorder() const { return showBorder_; }
	bool& showBorder() { return showBorder_; }

	const KpPen& borderPen() const { return borderPen_; }
	KpPen& borderPen() { return borderPen_; }

protected:

	void drawDiscreted_(KvPaint*, const KvDiscreted*) const override;

	using GETTER = std::function<std::vector<float_t>(unsigned ix, unsigned iy)>;

	virtual void drawImpl_(KvPaint*, GETTER, unsigned nx, unsigned ny, unsigned ch) const;

private:
	bool showFill_{ true };
	bool showBorder_{ false };
	KpPen borderPen_;
};
