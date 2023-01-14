#pragma once
#include "KvPlottable2d.h"

// 热图：实质为flat渲染模式的grid图（偏移dx/2, -dy/2）

class KcHeatMap : public KvPlottable2d
{
	using super_ = KvPlottable2d;

public:

	KcHeatMap(const std::string_view& name);

	aabb_t boundingBox() const override;

	bool showText() const { return showText_; }
	bool& showText() { return showText_; }

	color4f textColor() const { return clrText_; }
	color4f& textColor() { return clrText_; }

private:

	void drawImpl_(KvPaint*, point_getter2, unsigned nx, unsigned ny, unsigned c) const final;

private:

	bool showText_{ true };
	color4f clrText_{ 1, 0, 0, 1 };
};
