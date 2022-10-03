#pragma once
#include "KtColor.h"


// 一个简单的3d绘制接口

class KvPaint
{
public:

	virtual void setColor(const color4f& clr) = 0;

	virtual void setLineWidth(double width) = 0;

	virtual void drawLine(const point3d& from, const point3d& to) = 0;

	virtual void drawText(const point3d& anchor, const std::string& text, int align) = 0;

private:

};
