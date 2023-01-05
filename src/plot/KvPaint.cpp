#include "KvPaint.h"
#include "layout/KeAlignment.h"


void KvPaint::drawPoints(const point3 pts[], unsigned count)
{
	auto getter = [pts](unsigned idx) {
		return pts[idx];
	};

	drawPoints(getter, count);
}


void KvPaint::drawPoints(point_getter fn, unsigned count)
{
	for (unsigned i = 0; i < count; i++)
		drawPoint(fn(i));
}


void KvPaint::drawLineStrip(const point3 pts[], unsigned count)
{
	auto getter = [pts](unsigned idx) {
		return pts[idx];
	};

	drawLineStrip(getter, count);
}


void KvPaint::drawLineStrip(point_getter fn, unsigned count)
{
	for (unsigned i = 1; i < count; i++)
		drawLine(fn(i - 1), fn(i)); // TODO: 待优化，1个point2次调用fn
}


void KvPaint::drawLineLoop(const point3 pts[], unsigned count)
{
	auto getter = [pts](unsigned idx) {
		return pts[idx];
	};

	drawLineLoop(getter, count);
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


void KvPaint::drawBox(const point3& lower, const point3& upper)
{
	// TODO: 
	drawRect(lower, upper);
}


void KvPaint::fillQuad(point3 pts[4])
{
	fillTriangle(pts);
	std::swap(pts[0], pts[1]);
	fillTriangle(pts + 1);
}


void KvPaint::fillQuad(point3 pts[4], color_t clrs[4])
{
	fillTriangle(pts, clrs);
	std::swap(pts[0], pts[1]); std::swap(clrs[0], clrs[1]);
	fillTriangle(pts + 1, clrs + 1);
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


void KvPaint::apply(const KpFont& cxt)
{
	// TODO:
}
