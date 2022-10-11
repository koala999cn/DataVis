#pragma once
#include "KtColor.h"
#include <functional>
#include "KvRenderable.h"


// 一个简单的3d绘制接口

class KvPaint3d
{
public:
	using color_t = color4f;
	using float_type = typename KvRenderable::float_type;
	using point3 = typename KvRenderable::point3;
	using point_getter = std::function<point3(unsigned)>;

	virtual void setColor(const color_t& clr) = 0;

	virtual void setPointSize(double size) = 0;

	virtual void setLineWidth(double width) = 0;

	virtual void drawPoint(const point3& pt) = 0;

	virtual void drawPoints(const point3 pts[], unsigned count);

	void drawPoints(point_getter fn, unsigned count);

	virtual void drawLine(const point3& from, const point3& to) = 0;

	virtual void drawLineStrip(const point3 pts[], unsigned count);

	void drawLineStrip(point_getter fn, unsigned count);

	virtual void drawLineLoop(const point3 pts[], unsigned count);

	void drawLineLoop(point_getter fn, unsigned count);

	virtual void drawText(const point3& anchor, const char* text, int align) = 0;

private:

};
