#pragma once
#include "KtColor.h"


// 一个简单的3d绘制接口

class KvPaint
{
public:
	using color_t = color4f;
	using point3 = point3f;

	virtual void setColor(const color_t& clr) = 0;

	virtual void setPointSize(float size) = 0;

	virtual void setLineWidth(float width) = 0;

	virtual void drawPoint(const point3& pos) = 0;

	virtual void drawLine(const point3& from, const point3& to) = 0;

	virtual void drawText(const point3& anchor, const char* text, int align) = 0;

private:

};
