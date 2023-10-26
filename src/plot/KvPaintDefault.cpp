#include "KvPaintDefault.h"


void KvPaintDefault::beginPaint()
{
	assert(coords_.size() == 1 && coords_.back() == k_coord_local);
	cam_.updateProjectMatrixs();
}


void KvPaintDefault::endPaint()
{
	// depth sorting
	assert(coords_.size() == 1 && coords_.back() == k_coord_local);
}


KvPaintDefault::rect_t KvPaintDefault::viewport() const
{
	return cam_.viewport();
}


void KvPaintDefault::setViewport(const rect_t& vp)
{
	cam_.setViewport(vp);
}


void KvPaintDefault::pushLocal(const mat4& mat)
{
	cam_.pushLocal(mat);
}


void KvPaintDefault::popLocal()
{
	cam_.popLocal();
}


void KvPaintDefault::pushCoord(KeCoordType type)
{
	coords_.push_back(type);
}


void KvPaintDefault::popCoord()
{
	assert(coords_.size() > 1);
	coords_.pop_back();
}


KvPaintDefault::KeCoordType KvPaintDefault::currentCoord() const
{
	return KeCoordType(coords_.back());
}


KvPaintDefault::point4 KvPaintDefault::project(const point4& pt) const
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


KvPaintDefault::point4 KvPaintDefault::unproject(const point4& pt) const
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


KvPaintDefault::point4 KvPaintDefault::localToWorld(const point4& pt) const
{
	return cam_.localToWorld(pt);
}


KvPaintDefault::point4 KvPaintDefault::worldToLocal(const point4& pt) const
{
	return cam_.worldToLocal(pt);
}


void KvPaintDefault::setColor(const color_t& clr)
{
	clr_ = clr;
}


void KvPaintDefault::setSecondaryColor(const color_t& clr)
{
	secondaryClr_ = clr;
}


void KvPaintDefault::setMarkerSize(float_t size)
{
	markerSize_ = size;
}


void KvPaintDefault::setMarkerType(int type)
{
	markerType_ = type;
}


void KvPaintDefault::setLineWidth(float_t width)
{
	lineWidth_ = width;
}


void KvPaintDefault::setLineStyle(int style)
{
	lineStyle_ = style;
}


void KvPaintDefault::setFilled(bool b)
{
	filled_ = b;
}


void KvPaintDefault::setEdged(bool b)
{
	edged_ = b;
}


void KvPaintDefault::setFontFamily(const std::string_view& f)
{
	family_ = f;
}


void KvPaintDefault::setFontSize(int size)
{
	ftSize_ = size;
}


void KvPaintDefault::setBold(bool bold)
{
	bold_ = bold;
}


void KvPaintDefault::setItalic(bool italic)
{
	italic_ = italic;
}


void KvPaintDefault::setUnderline(bool underline)
{
	underline_ = underline;
}
