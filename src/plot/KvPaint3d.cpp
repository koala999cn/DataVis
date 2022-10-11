#include "KvPaint3d.h"


void KvPaint3d::drawPoints(const point3 pts[], unsigned count)
{
	for (unsigned i = 0; i < count; i++)
		drawPoint(pts[i]);
}


void KvPaint3d::drawPoints(point_getter fn, unsigned count)
{
	for (unsigned i = 0; i < count; i++)
		drawPoint(fn(i));
}


void KvPaint3d::drawLineStrip(const point3 pts[], unsigned count)
{
	for (unsigned i = 1; i < count; i++)
		drawLine(pts[i - 1], pts[i]);
}


void KvPaint3d::drawLineStrip(point_getter fn, unsigned count)
{
	for (unsigned i = 1; i < count; i++)
		drawLine(fn(i - 1), fn(i)); // TODO: 待优化，1个point2次调用fn
}


void KvPaint3d::drawLineLoop(const point3 pts[], unsigned count)
{
	drawLineStrip(pts, count);
	if (count  > 2)
	    drawLine(pts[count - 1], pts[0]);
}


void KvPaint3d::drawLineLoop(point_getter fn, unsigned count) 
{
	drawLineStrip(fn, count);
	if (count > 2)
		drawLine(fn(count - 1), fn(0));
}
