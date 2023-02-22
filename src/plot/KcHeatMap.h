#pragma once
#include "KvPlottable2d.h"

// 热图：实质为flat渲染模式的grid图（偏移dx/2, -dy/2）

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

	void* drawImpl_(KvPaint*, GETTER, unsigned nx, unsigned ny, unsigned c) const final;

	void* drawText_(KvPaint*, GETTER, unsigned nx, unsigned ny, unsigned c) const;

	aabb_t calcBoundingBox_() const override;

private:

	bool showText_{ true };
	color4f clrText_{ 1, 0, 0, 1 };
};
