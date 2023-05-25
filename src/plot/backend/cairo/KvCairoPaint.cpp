#include "KvCairoPaint.h"
#include "cairo/cairo.h"
#include "KuUtf8.h"

#define CAIRO_CTX reinterpret_cast<cairo_t*>(cxt_)
#define CAIRO_SURF reinterpret_cast<cairo_surface_t*>(surf_)


KvCairoPaint::~KvCairoPaint()
{
	destroy_();
}


void KvCairoPaint::destroy_()
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


void KvCairoPaint::addPath_(point_getter fn, unsigned count)
{
	auto pt = projectp(fn(0));
	cairo_move_to(CAIRO_CTX, pt.x(), pt.y());
	for (unsigned i = 1; i < count; i++) {
		pt = projectp(fn(i));
		cairo_line_to(CAIRO_CTX, pt.x(), pt.y());
	}
}


void KvCairoPaint::closePath_()
{
	cairo_close_path(CAIRO_CTX);
}


void KvCairoPaint::stroke_()
{
	applyLineCxt_();
	cairo_stroke(CAIRO_CTX);
}


void KvCairoPaint::fill_()
{
	setColor_();
	cairo_fill(CAIRO_CTX);
}


void KvCairoPaint::tryFillAndStroke_()
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


void KvCairoPaint::setColor_(const color4f& clr)
{
	cairo_set_source_rgba(CAIRO_CTX, clr.r(), clr.g(), clr.b(), clr.a());
}


void KvCairoPaint::setRect(const rect_t& rc)
{
	destroy_();

	canvas_ = rc; // 须在调用createSurface_之前设置canvas_
	surf_ = createSurface_();
	cxt_ = cairo_create(CAIRO_SURF);

	setViewport(rc);

	cairo_translate(CAIRO_CTX, -rc.lower().x(), -rc.lower().y());

	//cairo_select_font_face(CAIRO_CTX, u8"黑体", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(CAIRO_CTX, 13.0);
}


void KvCairoPaint::clear()
{
	setColor_();
	cairo_set_operator(CAIRO_CTX, CAIRO_OPERATOR_SOURCE);
	cairo_paint(CAIRO_CTX);
}


void KvCairoPaint::applyLineCxt_()
{
	setColor_();
	cairo_set_line_width(CAIRO_CTX, lineWidth_);
	// TODO: cairo_set_dash();
}


void KvCairoPaint::drawLine(const point3& from, const point3& to)
{
	auto pt0 = projectp(from), pt1 = projectp(to);
	cairo_move_to(CAIRO_CTX, pt0.x(), pt0.y());
	cairo_line_to(CAIRO_CTX, pt1.x(), pt1.y());
	stroke_();
}


void KvCairoPaint::drawRect(const point3& lower, const point3& upper)
{
	auto pt0 = projectp(lower), pt1 = projectp(upper);
	cairo_rectangle(CAIRO_CTX, pt0.x(), pt0.y(), pt1.x() - pt0.x(), pt1.y() - pt0.y());
	stroke_();
}


void KvCairoPaint::fillRect(const point3& lower, const point3& upper)
{
	auto pt0 = projectp(lower), pt1 = projectp(upper);
	cairo_rectangle(CAIRO_CTX, pt0.x(), pt0.y(), pt1.x() - pt0.x(), pt1.y() - pt0.y());
	fill_();
}


void* KvCairoPaint::drawLineStrips(const std::vector<point_getter>& fns, const std::vector<unsigned>& cnts)
{
	return nullptr;
}


void KvCairoPaint::drawMarker(const point3& pt)
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


void KvCairoPaint::fillTriangle(const point3 pts[3], const color_t clrs[3])
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


void KvCairoPaint::fillQuad(const point3 pts[4], const color_t clrs[4])
{
	if (flatShading()) {
		auto clrSaved = clr_;
		clr_ = clrs[3];
		fillQuad(pts);
		clr_ = clrSaved;
	}
	else {
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
}


void KvCairoPaint::gradLine_(const point3& st, const point3& ed, const color4f& c0, const color4f& c1)
{
	auto pt0 = projectp(st);
	auto pt1 = projectp(ed);
	auto pat = cairo_pattern_create_linear(pt0.x(), pt0.y(), pt1.x(), pt1.y());
	cairo_pattern_add_color_stop_rgba(pat, 0, c0.r(), c0.g(), c0.b(), c0.a());
	cairo_pattern_add_color_stop_rgba(pat, 1, c1.r(), c1.g(), c1.b(), c1.a());
	cairo_set_source(CAIRO_CTX, pat);
	cairo_move_to(CAIRO_CTX, pt0.x(), pt0.y());
	cairo_line_to(CAIRO_CTX, pt1.x(), pt1.y());
	cairo_stroke(CAIRO_CTX);
}


void KvCairoPaint::fillPoly(point_getter fn, unsigned count)
{
	if (count == 0)
		return;

	addPath_(fn, count);
	closePath_();
	fill_();
}


void* KvCairoPaint::fillBetween(point_getter line1, point_getter line2, unsigned count)
{
	return nullptr;
}


void KvCairoPaint::drawText(const point3& topLeft, const point3& hDir, const point3& vDir, const std::string_view& text)
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


namespace kPrivate
{
	template<typename T>
	T* next(T* p, unsigned stride) 
	{
		using CHAR_T = std::conditional_t<std::is_const_v<T>, const char*, char*>;
		return (T*)((CHAR_T)p + stride);
	}

	template<typename T>
	T* at(void* p, unsigned stride, unsigned idx)
	{
		return next<T>((T*)p, stride * idx);
	}
}

void* KvCairoPaint::drawGeom(vtx_decl_ptr decl, geom_ptr geom)
{
	auto vtx = (char*)geom->vertexData();
	auto nvtx = geom->vertexCount();
	auto stride = geom->vertexSize();
	auto idx = geom->indexData();
	auto nidx = geom->indexCount();
	auto vtxAttr = decl->findAttribute(KcVertexAttribute::k_position, 0);
	auto clrAttr = decl->findAttribute(KcVertexAttribute::k_diffuse, 0);

	assert(vtxAttr->format() == KcVertexAttribute::k_float3);
	assert(!idx || geom->indexSize() == 4);
	assert(!clrAttr || clrAttr->format() == KcVertexAttribute::k_float4);
	auto vtxp = (const float3*)(vtx + vtxAttr->offset());
	const color4f* clrp = clrAttr ? (const color4f*)(vtx + clrAttr->offset()) : nullptr;
	const std::uint32_t* idxp = idx ? (const std::uint32_t*)idx : nullptr;

	switch (geom->type()) {
	case k_quads:
	{
		unsigned nquad = idxp ? nidx / 4 : nvtx / 4;
		point3 quads[4];
		color4f clrs[4];
		for (unsigned i = 0; i < nquad; i++) {
			if (idxp) {
				for (unsigned j = 0; j < 4; j++)
					quads[j] = *kPrivate::at<const float3>((void*)vtxp, stride, idxp[4 * i + j]);

				if (clrp) {
					for (unsigned j = 0; j < 4; j++)
						clrs[j] = *kPrivate::at<const color4f>((void*)clrp, stride, idxp[4 * i + j]);
				}
			}
			else {
				for (unsigned j = 0; j < 4; j++) {
					quads[j] = *vtxp;
					vtxp = kPrivate::next<const float3>(vtxp, stride);
				}

				if (clrp) {
					for (unsigned j = 0; j < 4; j++) {
						clrs[j] = *clrp;
						clrp = kPrivate::next<const color4f>(clrp, stride);
					}
				}
			}

			if (clrp) {
				if (filled())
				    fillQuad(quads, clrs);
				
				if (edged()) {
					if (!filled()) { // 多色stroke
						applyLineCxt_();
						for (int i = 0; i < 4; i++)
							gradLine_(quads[i], quads[(i + 1) % 4], clrs[i], clrs[(i + 1) % 4]);
					}
					else { // 单色stroke
						addPath_(pointGetter(quads), 4);
						closePath_();
						stroke_();
					}
				}
			}
			else {
				addPath_(pointGetter(quads), 4);
				closePath_();
				tryFillAndStroke_(); // TODO: 可放到最后进行批量绘制
			}
		}
	}
	break;
	}

	return nullptr;
}


void KvCairoPaint::grab(int x, int y, int width, int height, void* data)
{

}


double KvCairoPaint::fontHeight() const
{
	cairo_font_extents_t fe;
	cairo_font_extents(CAIRO_CTX, &fe);
	return fe.height;
}


double KvCairoPaint::charSpacing() const
{
	return 1.;
}


KvCairoPaint::point2 KvCairoPaint::textSize(const std::string_view& text) const
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

