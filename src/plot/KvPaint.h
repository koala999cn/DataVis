#pragma once
#include "KtColor.h"


// һ���򵥵�3d���ƽӿ�

class KvPaint
{
public:

	virtual void setColor(const color4f& clr) = 0;

	virtual void setLineWidth(double width) = 0;

	virtual void drawLine(const point3d& from, const point3d& to) = 0;

	virtual void drawText(const point3d& anchor, const std::string& text, int align) = 0;

private:

};
