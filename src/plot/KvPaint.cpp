#include "KvPaint.h"


void KvPaint::drawPoints(const point3 pts[], unsigned count)
{
	for (unsigned i = 0; i < count; i++)
		drawPoint(pts[i]);
}


void KvPaint::drawPoints(point_getter fn, unsigned count)
{
	for (unsigned i = 0; i < count; i++)
		drawPoint(fn(i));
}


void KvPaint::drawLineStrip(const point3 pts[], unsigned count)
{
	for (unsigned i = 1; i < count; i++)
		drawLine(pts[i - 1], pts[i]);
}


void KvPaint::drawLineStrip(point_getter fn, unsigned count)
{
	for (unsigned i = 1; i < count; i++)
		drawLine(fn(i - 1), fn(i)); // TODO: 待优化，1个point2次调用fn
}


void KvPaint::drawLineLoop(const point3 pts[], unsigned count)
{
	drawLineStrip(pts, count);
	if (count  > 2)
	    drawLine(pts[count - 1], pts[0]);
}


void KvPaint::drawLineLoop(point_getter fn, unsigned count) 
{
	drawLineStrip(fn, count);
	if (count > 2)
		drawLine(fn(count - 1), fn(0));
}


void KvPaint::drawRect(const point3& lower, const point3& upper)
{
	point3 pts[4];
	pts[0] = lower, pts[2] = upper;
	pts[1] = { lower.x(), upper.y(), lower.z() };
	pts[3] = { upper.x(), lower.y(), upper.z() };

	drawLineLoop(pts, 4);
}


KvPaint::rect KvPaint::textRect(const point2& pos, const char* text, int align) const
{
	auto lower = pos;
	auto szText = textSize(text);

	if (align & k_align_bottom)
		lower.y() -= szText.y();
	else if (align & k_align_top)
		lower.y();
	else // k_align_venter
		lower.y() -= szText.y() * 0.5;

	if (align & k_align_left)
		lower.x();
	else if (align & k_align_right)
		lower.x() -= szText.x();
	else // k_align_center
		lower.x() -= szText.x() * 0.5;

	return { lower, lower + szText };
}


void KvPaint::apply(const KpPen& cxt)
{
	setColor(cxt.color);
	setLineWidth(cxt.width);
	setLineStyle(cxt.style);
}


void KvPaint::apply(const KpBrush& cxt)
{
	setColor(cxt.color);
}
