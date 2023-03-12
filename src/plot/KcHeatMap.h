#pragma once
#include "KvPlottable2d.h"

// 热图：实质为flat渲染模式 + text绘制 + 平面化的surface图（偏移dx/2, -dy/2）

class KcHeatMap : public KvPlottable2d
{
	using super_ = KvPlottable2d;

public:

	KcHeatMap(const std::string_view& name);

	bool showText() const { return showText_; }
	bool& showText() { return showText_; }

	color4f textColor() const { return clrText_; }
	color4f& textColor() { return clrText_; }

private:

	unsigned objectCount() const override;

	bool objectVisible_(unsigned objIdx) const override;

	void setObjectState_(KvPaint*, unsigned objIdx) const override;

	void* drawObject_(KvPaint*, unsigned objIdx) const final;

	void* drawGrid_(KvPaint*) const;

	void* drawText_(KvPaint*) const;

	aabb_t calcBoundingBox_() const override;

	// 计算xy平面的偏移量
	std::pair<float_t, float_t> xyshift_() const;

private:

	bool showText_{ true };
	color4f clrText_{ 1, 0, 0, 1 };
};
