#include "KcCairoPaint.h"
#include "cairo/cairo.h"
#include "cairo/cairo-svg.h" // TODO:
#include "KuUtf8.h"

#define CAIRO_CTX reinterpret_cast<cairo_t*>(cxt_)
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
		cairo_destroy(CAIRO_CTX);
		cxt_ = nullptr;
	}
}


void KcCairoPaint::addPath_(point_getter fn, unsigned count)
{
	auto pt = projectp(fn(0));
	cairo_move_to(CAIRO_CTX, pt.x(), pt.y());
	for (unsigned i = 1; i < count; i++) {
		pt = projectp(fn(i));
		cairo_line_to(CAIRO_CTX, pt.x(), pt.y());
	}
}


void KcCairoPaint::closePath_()
{
	cairo_close_path(CAIRO_CTX);
}


void KcCairoPaint::stroke_()
{
	applyLineCxt_();
	cairo_stroke(CAIRO_CTX);
}


void KcCairoPaint::fill_()
{
	setColor_();
	cairo_fill(CAIRO_CTX);
}


void KcCairoPaint::tryFillAndStroke_()
{
	if (filled()) {
		setColor_();
		cairo_fill_preserve(CAIRO_CTX);
	}

	if (edged()) {
		auto clr = clr_;
		if (filled())
			clr_ = secondaryClr_;
		stroke_();
		clr_ = clr;
	}

	cairo_new_path(CAIRO_CTX); // fill调用了preserve版本
}


void KcCairoPaint::setColor_(const color4f& clr)
{
	cairo_set_source_rgba(CAIRO_CTX, clr.r(), clr.g(), clr.b(), clr.a());
}


void KcCairoPaint::setRect(const rect_t& rc)
{
	destroy_();
	//surf_ = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
	surf_ = cairo_svg_surface_create("test.svg", rc.width(), rc.height());
	cxt_ = cairo_create(CAIRO_SURF);


	cairo_pattern_t* pattern = cairo_pattern_create_mesh();

	canvas_ = rc;
	setViewport(rc);

	cairo_translate(CAIRO_CTX, -rc.lower().x(), -rc.lower().y());

	//cairo_select_font_face(CAIRO_CTX, u8"黑体", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(CAIRO_CTX, 13.0);


	/* 
	cairo_mesh_pattern_begin_patch(pattern);
	cairo_mesh_pattern_line_to(pattern, rc.upper().x(), rc.upper().y());
	cairo_mesh_pattern_line_to(pattern, rc.upper().x(), rc.lower().y());
	cairo_mesh_pattern_line_to(pattern, rc.lower().x(), rc.lower().y());
	cairo_mesh_pattern_line_to(pattern, rc.lower().x(), rc.upper().y());
	cairo_mesh_pattern_set_corner_color_rgb(pattern, 0, 1, 0, 0);
	cairo_mesh_pattern_set_corner_color_rgb(pattern, 1, 0, 1, 0);
	cairo_mesh_pattern_set_corner_color_rgb(pattern, 2, 0, 0, 1);
	cairo_mesh_pattern_set_corner_color_rgb(pattern, 3, 0, 1, 1);
	cairo_mesh_pattern_end_patch(pattern);
	cairo_set_source(CAIRO_CTX, pattern);
	cairo_paint(CAIRO_CTX);
	cairo_pattern_destroy(pattern);*/
}


void KcCairoPaint::clear()
{
	setColor_();
	cairo_set_operator(CAIRO_CTX, CAIRO_OPERATOR_SOURCE);
	cairo_paint(CAIRO_CTX);
}


void KcCairoPaint::applyLineCxt_()
{
	setColor_();
	cairo_set_line_width(CAIRO_CTX, lineWidth_);
	// TODO: cairo_set_dash();
}


void KcCairoPaint::drawLine(const point3& from, const point3& to)
{
	auto pt0 = projectp(from), pt1 = projectp(to);
	cairo_move_to(CAIRO_CTX, pt0.x(), pt0.y());
	cairo_line_to(CAIRO_CTX, pt1.x(), pt1.y());
	stroke_();
}


void KcCairoPaint::drawRect(const point3& lower, const point3& upper)
{
	auto pt0 = projectp(lower), pt1 = projectp(upper);
	cairo_rectangle(CAIRO_CTX, pt0.x(), pt0.y(), pt1.x() - pt0.x(), pt1.y() - pt0.y());
	stroke_();
}


void KcCairoPaint::fillRect(const point3& lower, const point3& upper)
{
	auto pt0 = projectp(lower), pt1 = projectp(upper);
	cairo_rectangle(CAIRO_CTX, pt0.x(), pt0.y(), pt1.x() - pt0.x(), pt1.y() - pt0.y());
	fill_();
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
	    cairo_arc(CAIRO_CTX, p.x(), p.y(), markerSize(), 0, KuMath::pi * 2);
		tryFillAndStroke_();
	}
	else {
		return super_::drawMarker(pt);
	}
}


void KcCairoPaint::fillTriangle(const point3 pts[3], const color_t clrs[3])
{
	auto pat = cairo_pattern_create_mesh();
	cairo_mesh_pattern_begin_patch(pat);
	for (int i = 0; i < 3; i++) {
		auto pt = projectp(pts[i]);
		cairo_mesh_pattern_line_to(pat, pt.x(), pt.y());
		cairo_mesh_pattern_set_corner_color_rgba(pat, i, clrs[i].r(), clrs[i].g(), clrs[i].b(), clrs[i].a());
	}
	cairo_mesh_pattern_end_patch(pat);
	cairo_set_source(CAIRO_CTX, pat);
	cairo_paint(CAIRO_CTX);
	cairo_pattern_destroy(pat);
}


void KcCairoPaint::fillQuad(const point3 pts[4], const color_t clrs[4])
{
	auto pat = cairo_pattern_create_mesh();
	cairo_mesh_pattern_begin_patch(pat);
	for (int i = 0; i < 4; i++) {
		auto pt = projectp(pts[i]);
		cairo_mesh_pattern_line_to(pat, pt.x(), pt.y());
		cairo_mesh_pattern_set_corner_color_rgba(pat, i, clrs[i].r(), clrs[i].g(), clrs[i].b(), clrs[i].a());
	}
	cairo_mesh_pattern_end_patch(pat);

	cairo_set_source(CAIRO_CTX, pat);
	cairo_paint(CAIRO_CTX);
	cairo_pattern_destroy(pat);
}


void KcCairoPaint::fillPoly(point_getter fn, unsigned count)
{
	if (count == 0)
		return;

	addPath_(fn, count);
	closePath_();
	fill_();
}


void* KcCairoPaint::fillBetween(point_getter line1, point_getter line2, unsigned count)
{
	return nullptr;
}


void KcCairoPaint::drawText(const point3& topLeft, const point3& hDir, const point3& vDir, const std::string_view& text)
{
	setColor_();

	auto pt = projectp(topLeft);

	cairo_font_extents_t fe;
	cairo_font_extents(CAIRO_CTX, &fe);

	KuUtf8::forEach(text, [this, &pt, &fe](const char* ch) {
		cairo_text_extents_t te;
		cairo_text_extents(CAIRO_CTX, ch, &te);

		cairo_move_to(CAIRO_CTX, pt.x() - te.x_bearing, pt.y() + fe.height - fe.descent);
		//cairo_move_to(CAIRO_CTX, pt.x() - te.x_bearing, pt.y() + 13. / 2 - te.y_bearing - te.height / 2);
		pt.x() += te.width + charSpacing();
		cairo_show_text(CAIRO_CTX, ch);
		});

	cairo_new_path(CAIRO_CTX);
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
	cairo_font_extents(CAIRO_CTX, &fe);
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
		cairo_text_extents(CAIRO_CTX, ch, &te);
		width += te.width;
		++nchs;
		});

	return { nchs == 0 ? 0 : width + (nchs - 1) * charSpacing(), fontHeight() };
}

