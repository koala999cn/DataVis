#include "KcCairoPaint.h"
#include "cairo/cairo.h"
#include "cairo/cairo-svg.h" // TODO:
#include "KuUtf8.h"

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


void KcCairoPaint::setRect(const rect_t& rc)
{
	destroy_();
	//surf_ = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
	surf_ = cairo_svg_surface_create("test.svg", rc.width(), rc.height());
	cxt_ = cairo_create(CAIRO_SURF);
	cairo_translate(CAIRO_CR, -rc.lower().x(), -rc.lower().y());
	canvas_ = rc;
	setViewport(rc);

	//cairo_select_font_face(CAIRO_CR, u8"ºÚÌå", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(CAIRO_CR, 13.0);
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
	cairo_new_path(CAIRO_CR);
	cairo_move_to(CAIRO_CR, pt0.x(), pt0.y());
	cairo_line_to(CAIRO_CR, pt1.x(), pt1.y());
	cairo_stroke(CAIRO_CR);
}


void* KcCairoPaint::drawLineStrips(const std::vector<point_getter>& fns, const std::vector<unsigned>& cnts)
{
	return nullptr;
}


void KcCairoPaint::drawMarker(const point3& pt)
{
	if (markerType() == KpMarker::k_circle ||
		markerType() == KpMarker::k_dot) {
		auto p = projectp(pt);
		cairo_new_path(CAIRO_CR);
	    cairo_arc(CAIRO_CR, p.x(), p.y(), markerSize(), 0, KuMath::pi * 2);
		cairo_set_source_rgba(CAIRO_CR, clr_.r(), clr_.g(), clr_.b(), clr_.a());

		if (filled()) 
			cairo_fill(CAIRO_CR);
		
		if (edged()) {
			if (filled()) {
				cairo_arc(CAIRO_CR, p.x(), p.y(), markerSize(), 0, KuMath::pi * 2);
				cairo_set_source_rgba(CAIRO_CR, secondaryClr_.r(), secondaryClr_.g(), secondaryClr_.b(), secondaryClr_.a());
			}
			cairo_set_line_width(CAIRO_CR, lineWidth_);
			cairo_stroke(CAIRO_CR);
		}
	}
	else {
		return super_::drawMarker(pt);
	}
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


void KcCairoPaint::fillPoly(point_getter fn, unsigned count)
{
	if (count == 0)
		return;

	cairo_set_source_rgba(CAIRO_CR, clr_.r(), clr_.g(), clr_.b(), clr_.a());

	cairo_new_path(CAIRO_CR);
	auto pt = projectp(fn(0));
	cairo_move_to(CAIRO_CR, pt.x(), pt.y());
	for (unsigned i = 0; i < count; i++) {
		pt = projectp(fn(i));
		cairo_line_to(CAIRO_CR, pt.x(), pt.y());
	}
	cairo_close_path(CAIRO_CR);
	cairo_fill(CAIRO_CR);
}


void* KcCairoPaint::fillBetween(point_getter line1, point_getter line2, unsigned count)
{
	return nullptr;
}


void KcCairoPaint::drawText(const point3& topLeft, const point3& hDir, const point3& vDir, const std::string_view& text)
{
	cairo_set_source_rgba(CAIRO_CR, clr_.r(), clr_.g(), clr_.b(), clr_.a());

	auto pt = projectp(topLeft);

	cairo_font_extents_t fe;
	cairo_font_extents(CAIRO_CR, &fe);

	KuUtf8::forEach(text, [this, &pt, &fe](const char* ch) {
		cairo_text_extents_t te;
		cairo_text_extents(CAIRO_CR, ch, &te);

		cairo_move_to(CAIRO_CR, pt.x() - te.x_bearing, pt.y() + fe.height - fe.descent);
		//cairo_move_to(CAIRO_CR, pt.x() - te.x_bearing, pt.y() + 13. / 2 - te.y_bearing - te.height / 2);
		pt.x() += te.width + charSpacing();
		cairo_show_text(CAIRO_CR, ch);
		});
}


void* KcCairoPaint::drawGeom(vtx_decl_ptr decl, geom_ptr geom)
{
	return nullptr;
}


void KcCairoPaint::grab(int x, int y, int width, int height, void* data)
{

}


double KcCairoPaint::fontHeight() const
{
	cairo_font_extents_t fe;
	cairo_font_extents(CAIRO_CR, &fe);
	return fe.height;
}


double KcCairoPaint::charSpacing() const
{
	return 1.;
}


KcCairoPaint::point2 KcCairoPaint::textSize(const std::string_view& text) const
{
	double width(0); int nchs(0);
	KuUtf8::forEach(text, [this, &width, &nchs](const char* ch) {
		cairo_text_extents_t te;
		cairo_text_extents(CAIRO_CR, ch, &te);
		width += te.width;
		++nchs;
		});

	return { nchs == 0 ? 0 : width + (nchs - 1) * charSpacing(), fontHeight() };
}
