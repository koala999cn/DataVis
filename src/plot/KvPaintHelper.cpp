#include "KvPaintHelper.h"


void KvPaintHelper::beginPaint()
{
	assert(coords_.size() == 1 && coords_.back() == k_coord_local);
	cam_.updateProjectMatrixs();
}


void KvPaintHelper::endPaint()
{
	// depth sorting
	assert(coords_.size() == 1 && coords_.back() == k_coord_local);
}


KvPaintHelper::rect_t KvPaintHelper::viewport() const
{
	return cam_.viewport();
}


void KvPaintHelper::setViewport(const rect_t& vp)
{
	cam_.setViewport(vp);
}


void KvPaintHelper::pushClipRect(const rect_t& cr)
{

}


void KvPaintHelper::popClipRect()
{

}


void KvPaintHelper::pushLocal(const mat4& mat)
{
	cam_.pushLocal(mat);
}


void KvPaintHelper::popLocal()
{
	cam_.popLocal();
}


void KvPaintHelper::pushCoord(KeCoordType type)
{
	coords_.push_back(type);
}


void KvPaintHelper::popCoord()
{
	assert(coords_.size() > 1);
	coords_.pop_back();
}


KvPaintHelper::KeCoordType KvPaintHelper::currentCoord() const
{
	return KeCoordType(coords_.back());
}


KvPaintHelper::point4 KvPaintHelper::project(const point4& pt) const
{
	switch (coords_.back())
	{
	case k_coord_local:
		return cam_.localToScreen(pt).homogenize(); // NB: 透视投影须作归一化

	case k_coord_world:
		return cam_.worldToScreen(pt).homogenize(); // NB: 透视投影须作归一化

	case k_coord_local_screen:
		return cam_.localToWorld(pt); // 仅执行局部变换

	case k_coord_screen:
		return pt;

	case k_coord_ndc:
		return cam_.ndcToScreen(pt);

	default:
		break;
	}

	assert(false);
	return pt;
}


KvPaintHelper::point4 KvPaintHelper::unproject(const point4& pt) const
{
	switch (coords_.back())
	{
	case k_coord_local:
		return cam_.screenToLocal(pt).homogenize();

	case k_coord_world:
		return cam_.screenToWorld(pt).homogenize();

	case k_coord_local_screen:
		return cam_.worldToLocal(pt); // 仅执行局部变换

	case k_coord_screen:
		return pt;

	case k_coord_ndc:
		return cam_.screenToNdc(pt);

	default:
		break;
	}

	assert(false);
	return pt;
}


KvPaintHelper::point4 KvPaintHelper::localToWorld(const point4& pt) const
{
	return cam_.localToWorld(pt);
}


KvPaintHelper::point4 KvPaintHelper::worldToLocal(const point4& pt) const
{
	return cam_.worldToLocal(pt);
}


void KvPaintHelper::setColor(const color_t& clr)
{
	clr_ = clr;
}


void KvPaintHelper::setSecondaryColor(const color_t& clr)
{
	secondaryClr_ = clr;
}


void KvPaintHelper::setMarkerSize(float_t size)
{
	markerSize_ = size;
}


void KvPaintHelper::setMarkerType(int type)
{
	markerType_ = type;
}


void KvPaintHelper::setLineWidth(float_t width)
{
	lineWidth_ = width;
}


void KvPaintHelper::setLineStyle(int style)
{
	lineStyle_ = style;
}


void KvPaintHelper::setFilled(bool b)
{
	filled_ = b;
}


void KvPaintHelper::setEdged(bool b)
{
	edged_ = b;
}

