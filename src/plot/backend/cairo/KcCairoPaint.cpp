#include "KcCairoPaint.h"
#include "cairo/cairo.h"
#include "cairo/cairo-svg.h" // TODO:

#define CAIRO_CR reinterpret_cast<cairo_t*>(cxt_)
#define CAIRO_SURF reinterpret_cast<cairo_surface_t*>(surf_)

KcCairoPaint::KcCairoPaint()
{

}


KcCairoPaint::~KcCairoPaint()
{
	destroy_();
}


void KcCairoPaint::destroy_()
{
	if (surf_) {
		cairo_surface_destroy(CAIRO_SURF);
		surf_ = nullptr;
	}

	if (cxt_) {
		cairo_destroy(CAIRO_CR);
		cxt_ = nullptr;
	}
}


void KcCairoPaint::beginPaint()
{
	///if (!canvas_.lower().isZero())
	//	pushLocal(mat4::buildTanslation(vec3d(-canvas_.lower().x(), -canvas_.lower().y(), 0)));

	super_::beginPaint();
}


void KcCairoPaint::endPaint()
{
	super_::endPaint();
	//popLocal();
}


void KcCairoPaint::setRect(const rect_t& rc)
{
	destroy_();
	//surf_ = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
	surf_ = cairo_svg_surface_create("test.svg", rc.width(), rc.height());
	cxt_ = cairo_create(CAIRO_SURF);
	cairo_translate(CAIRO_CR, -rc.lower().x(), -rc.lower().y());
	canvas_ = rc;
	setViewport(rc);
}


void KcCairoPaint::clear(const color4f& clr)
{
	cairo_save(CAIRO_CR);
	cairo_set_source_rgba(CAIRO_CR, clr.r(), clr.g(), clr.b(), clr.a());
	cairo_set_operator(CAIRO_CR, CAIRO_OPERATOR_SOURCE);
	cairo_paint(CAIRO_CR);
	cairo_restore(CAIRO_CR);
}


void KcCairoPaint::applyLineCxt_()
{
	cairo_set_source_rgba(CAIRO_CR, clr_.r(), clr_.g(), clr_.b(), clr_.a());
	cairo_set_line_width(CAIRO_CR, lineWidth_);
	// TODO: cairo_set_dash();
}


void KcCairoPaint::drawLine(const point3& from, const point3& to)
{
	applyLineCxt_();
	auto pt0 = projectp(from), pt1 = projectp(to);
	cairo_move_to(CAIRO_CR, pt0.x(), pt0.y());
	cairo_line_to(CAIRO_CR, pt1.x(), pt1.y());
	cairo_stroke(CAIRO_CR);
}


void KcCairoPaint::drawMarker(const point3& pt)
{

}


void* KcCairoPaint::drawMarkers(point_getter fn, color_getter clr, size_getter size, unsigned count)
{
	return nullptr;
}


void* KcCairoPaint::drawLineStrips(const std::vector<point_getter>& fns, const std::vector<unsigned>& cnts)
{
	return nullptr;
}


void KcCairoPaint::fillTriangle(point3 pts[3])
{

}


void KcCairoPaint::fillTriangle(point3 pts[3], color_t clrs[3])
{

}


void KcCairoPaint::fillRect(const point3& lower, const point3& upper)
{
	cairo_set_source_rgba(CAIRO_CR, clr_.r(), clr_.g(), clr_.b(), clr_.a());

	auto pt0 = projectp(lower), pt1 = projectp(upper);
	cairo_rectangle(CAIRO_CR, pt0.x(), pt0.y(), pt1.x() - pt0.x(), pt1.y() - pt0.y());
	cairo_fill(CAIRO_CR);
}


void KcCairoPaint::fillConvexPoly(point_getter fn, unsigned count)
{

}


void* KcCairoPaint::fillBetween(point_getter line1, point_getter line2, unsigned count)
{
	return nullptr;
}


void KcCairoPaint::drawText(const point3& topLeft, const point3& hDir, const point3& vDir, const char* text)
{

}


void KcCairoPaint::drawText(const point3& anchor, const char* text, int align)
{

}


void* KcCairoPaint::drawTexts(const std::vector<point3>& anchors, const std::vector<std::string>& texts, int align, const point2f& spacing)
{
	return nullptr;
}


void* KcCairoPaint::drawGeom(vtx_decl_ptr decl, geom_ptr geom)
{
	return nullptr;
}


void KcCairoPaint::grab(int x, int y, int width, int height, void* data)
{

}


KcCairoPaint::point2 KcCairoPaint::textSize(const char* text) const
{
	return { 7 * strlen(text), 13 };
}