#include "KvCairoPaint.h"
#include "cairo/cairo.h"
#include "KuUtf8.h"

#define CAIRO_CXT reinterpret_cast<cairo_t*>(cxt_)
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
		cairo_destroy(CAIRO_CXT);
		cxt_ = nullptr;
	}
}


void KvCairoPaint::addPath_(point_getter fn, unsigned count)
{
	auto pt = projectp(fn(0));
	cairo_move_to(CAIRO_CXT, pt.x(), pt.y());
	for (unsigned i = 1; i < count; i++) {
		pt = projectp(fn(i));
		cairo_line_to(CAIRO_CXT, pt.x(), pt.y());
	}
}


void KvCairoPaint::closePath_()
{
	cairo_close_path(CAIRO_CXT);
}


void KvCairoPaint::stroke_()
{
	applyLineCxt_();
	cairo_stroke(CAIRO_CXT);
}


void KvCairoPaint::fill_()
{
	setColor_();
	cairo_fill(CAIRO_CXT);
}


void KvCairoPaint::tryFillAndStroke_()
{
	if (filled()) {
		setColor_();
		cairo_fill_preserve(CAIRO_CXT);
	}

	if (edged()) {
		auto clr = clr_;
		if (filled())
			clr_ = secondaryClr_;
		stroke_();
		clr_ = clr;
	}

	cairo_new_path(CAIRO_CXT); // fill调用了preserve版本
}


void KvCairoPaint::setColor_(const color4f& clr)
{
	cairo_set_source_rgba(CAIRO_CXT, clr.r(), clr.g(), clr.b(), clr.a());
}


void KvCairoPaint::setRect(const rect_t& rc)
{
	destroy_();

	canvas_ = rc; // 须在调用createSurface_之前设置canvas_
	surf_ = createSurface_();
	cxt_ = cairo_create(CAIRO_SURF);

	setViewport(rc);

	cairo_translate(CAIRO_CXT, -rc.lower().x(), -rc.lower().y());

	//cairo_select_font_face(CAIRO_CXT, u8"黑体", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(CAIRO_CXT, 13.0);
}


void KvCairoPaint::clear()
{
	setColor_();
	cairo_set_operator(CAIRO_CXT, CAIRO_OPERATOR_SOURCE);
	cairo_paint(CAIRO_CXT);
}


void KvCairoPaint::setClipRect_()
{
	cairo_reset_clip(CAIRO_CXT);
	if (!clipRectStack_.empty()) {
		auto rc = clipRectStack_.back();
		point2 pts[4];
		pts[0] = rc.lower(), pts[2] = rc.upper();
		pts[1] = { rc.lower().x(), rc.upper().y() };
		pts[3] = { rc.upper().x(), rc.lower().y() };
		cairo_move_to(CAIRO_CXT, pts[0].x(), pts[0].y());
		for (int i = 1; i < 4; i++)
			cairo_line_to(CAIRO_CXT, pts[i].x(), pts[i].y());
		cairo_close_path(CAIRO_CXT);
		cairo_clip(CAIRO_CXT);
	}
}


void KvCairoPaint::pushClipRect(const rect_t& cr, bool reset)
{
	if (reset || clipRectStack_.empty())
		clipRectStack_.push_back(cr);
	else
		clipRectStack_.push_back(clipRectStack_.back().intersection(cr));

	setClipRect_();
}


void KvCairoPaint::popClipRect()
{
	clipRectStack_.pop_back();
	setClipRect_();
}


KvCairoPaint::rect_t KvCairoPaint::clipRect() const
{
	return clipRectStack_.empty() ? canvas_ : clipRectStack_.back();
}


void KvCairoPaint::enableClipBox(point3 lower, point3 upper)
{
	// TODO
}


void KvCairoPaint::disableClipBox()
{
	// TODO
}


void KvCairoPaint::applyLineCxt_()
{
	setColor_();
	applyLineStyle_();
	cairo_set_line_width(CAIRO_CXT, lineWidth_);
}


void KvCairoPaint::applyLineStyle_()
{
	const double* dashes = nullptr;
	int ndash = 0;

	if (lineStyle_ == KpPen::k_dot) {
		static const double dot[] = { 1, 1 };
		dashes = dot;
		ndash = 2;
	}
	else if (lineStyle_ == KpPen::k_dash) {
		static const double dash[] = { 2, 2 };
		dashes = dash;
		ndash = 2;
	}
	else if (lineStyle_ == KpPen::k_dash4) {
		static const double dash4[] = { 4, 4 };
		dashes = dash4;
		ndash = 2;
	}
	else if (lineStyle_ == KpPen::k_dash8) {
		static const double dash8[] = { 8, 8 };
		dashes = dash8;
		ndash = 2;
	}
	else if (lineStyle_ == KpPen::k_dash_dot) {
		static const double dashdot[] = { 5, 4, 1, 6 }; // 0xF840
		dashes = dashdot;
		ndash = 4;
	}
	else if (lineStyle_ == KpPen::k_dash_dot_dot) {
		static const double dashdotdot[] = { 5, 3, 1, 3, 1, 3 }; // 0xF888
		dashes = dashdotdot;
		ndash = 6;
	}

	cairo_set_dash(CAIRO_CXT, dashes, ndash, 0);
}


void KvCairoPaint::drawLine(const point3& from, const point3& to)
{
	auto pt0 = projectp(from), pt1 = projectp(to);
	cairo_move_to(CAIRO_CXT, pt0.x(), pt0.y());
	cairo_line_to(CAIRO_CXT, pt1.x(), pt1.y());
	stroke_();
}


void KvCairoPaint::drawRect(const point3& lower, const point3& upper)
{
	auto pt0 = projectp(lower), pt1 = projectp(upper);
	cairo_rectangle(CAIRO_CXT, pt0.x(), pt0.y(), pt1.x() - pt0.x(), pt1.y() - pt0.y());
	stroke_();
}


void KvCairoPaint::fillRect(const point3& lower, const point3& upper)
{
	auto pt0 = projectp(lower), pt1 = projectp(upper);
	cairo_rectangle(CAIRO_CXT, pt0.x(), pt0.y(), pt1.x() - pt0.x(), pt1.y() - pt0.y());
	fill_();
}


void* KvCairoPaint::drawLineStrips(const std::vector<point_getter>& fns, const std::vector<unsigned>& cnts)
{
	for (unsigned i = 0; i < fns.size(); i++)
	    addPath_(fns[i], cnts[i]);
	stroke_();
	return nullptr;
}


void KvCairoPaint::drawMarker(const point3& pt)
{
	if (markerType() == KpMarker::k_circle ||
		markerType() == KpMarker::k_dot) {
		auto p = projectp(pt);
	    cairo_arc(CAIRO_CXT, p.x(), p.y(), markerSize(), 0, KuMath::pi * 2);
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
	cairo_set_source(CAIRO_CXT, pat);
	cairo_paint(CAIRO_CXT);
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

		cairo_set_source(CAIRO_CXT, pat);
		cairo_paint(CAIRO_CXT);
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
	cairo_set_source(CAIRO_CXT, pat);
	cairo_move_to(CAIRO_CXT, pt0.x(), pt0.y());
	cairo_line_to(CAIRO_CXT, pt1.x(), pt1.y());
	cairo_stroke(CAIRO_CXT);
}


void KvCairoPaint::fillPoly(point_getter fn, unsigned count)
{
	if (count == 0)
		return;

	addPath_(fn, count);
	closePath_();
	fill_();
}


#include "KtGeometryImpl.h"
#include "KtLineS2d.h"
void* KvCairoPaint::fillBetween(point_getter fn1, point_getter fn2, unsigned count)
{
	auto geom = std::make_shared<KtGeometryImpl<float3>>(k_triangles);

	geom->reserve((count - 1) * 6, 0); // 每个区间绘制2个三角形，共6个顶点

	auto p00 = fn1(0);
	auto p01 = fn2(0);

	assert(p00.z() == p01.z()); // 要求各点都在一个z平面上

	for (unsigned i = 1; i < count; i++) {
		auto p10 = fn1(i);
		auto p11 = fn2(i);

		using point2 = KtPoint<float_t, 2>;
		KtLineS2d<float_t> ln0((const point2&)p00, (const point2&)p10);
		KtLineS2d<float_t> ln1((const point2&)p01, (const point2&)p11);
		auto pt = ln0.intersects(ln1);
		auto vtx = geom->newVertex(6);
		if (pt) { // 相交

			float3 ptm(pt->x(), pt->y(), p00.z());

			vtx[0] = p01;
			vtx[1] = p00;
			vtx[2] = ptm;

			vtx[3] = p10;
			vtx[4] = p11;
			vtx[5] = ptm;
		}
		else { // 不相交
			vtx[0] = p01;
			vtx[1] = p00;
			vtx[2] = p10;

			vtx[3] = p10;
			vtx[4] = p11;
			vtx[5] = p01;
		}

		p00 = p10, p01 = p11;
	}

	return drawGeomSolid(geom);
}


void KvCairoPaint::drawText(const point3& topLeft, const point3& hDir, const point3& vDir, const std::string_view& text)
{
	setColor_();

	auto pt = projectp(topLeft);

	cairo_font_extents_t fe;
	cairo_font_extents(CAIRO_CXT, &fe);

	KuUtf8::forEach(text, [this, &pt, &fe](const char* ch) {
		cairo_text_extents_t te;
		cairo_text_extents(CAIRO_CXT, ch, &te);

		cairo_move_to(CAIRO_CXT, pt.x() - te.x_bearing, pt.y() + fe.height - fe.descent);
		//cairo_move_to(CAIRO_CXT, pt.x() - te.x_bearing, pt.y() + 13. / 2 - te.y_bearing - te.height / 2);
		pt.x() += te.width + charSpacing();
		cairo_show_text(CAIRO_CXT, ch);
		});

	cairo_new_path(CAIRO_CXT);
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


void KvCairoPaint::drawMesh_(vtx_decl_ptr decl, geom_ptr geom, int nvtx)
{
	auto vtx = (char*)geom->vertexData();
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

	unsigned npoly = idxp ? nidx / nvtx : geom->vertexCount() / nvtx;
	std::vector<point3> polys(nvtx);
	std::vector<color4f> clrs(nvtx);
	for (unsigned i = 0; i < npoly; i++) {
		if (idxp) {
			for (unsigned j = 0; j < nvtx; j++)
				polys[j] = *kPrivate::at<const float3>((void*)vtxp, stride, idxp[nvtx * i + j]);

			if (clrp) {
				for (unsigned j = 0; j < nvtx; j++)
					clrs[j] = *kPrivate::at<const color4f>((void*)clrp, stride, idxp[nvtx * i + j]);
			}
		}
		else {
			for (unsigned j = 0; j < nvtx; j++) {
				polys[j] = *vtxp;
				vtxp = kPrivate::next<const float3>(vtxp, stride);
			}

			if (clrp) {
				for (unsigned j = 0; j < nvtx; j++) {
					clrs[j] = *clrp;
					clrp = kPrivate::next<const color4f>(clrp, stride);
				}
			}
		}

		if (clrp) {
			if (filled()) {
				if (nvtx == 3) // TODO: impl fillPoly
					fillTriangle(polys.data(), clrs.data());
				else 
				    fillQuad(polys.data(), clrs.data());
			}

			if (edged()) {
				if (!filled()) { // 多色stroke
					applyLineCxt_();
					for (int i = 0; i < nvtx; i++)
						gradLine_(polys[i], polys[(i + 1) % nvtx], clrs[i], clrs[(i + 1) % nvtx]);
				}
				else { // 单色stroke
					addPath_(pointGetter(polys.data()), nvtx);
					closePath_();
					stroke_();
				}
			}
		}
		else {
			addPath_(pointGetter(polys.data()), nvtx);
			closePath_();
			tryFillAndStroke_(); // TODO: 可放到最后进行批量绘制
		}
	}
}


void* KvCairoPaint::drawGeom(vtx_decl_ptr decl, geom_ptr geom)
{
	switch (geom->type()) 
	{
	case k_quads:
		drawMesh_(decl, geom, 4);
	    break;

	case k_triangles:
		drawMesh_(decl, geom, 3);
		break;

	default:
		assert(false); // TODO
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
	cairo_font_extents(CAIRO_CXT, &fe);
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
		cairo_text_extents(CAIRO_CXT, ch, &te);
		width += te.width;
		++nchs;
		});

	return { nchs == 0 ? 0 : width + (nchs - 1) * charSpacing(), fontHeight() };
}

