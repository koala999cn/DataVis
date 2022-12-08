#include "KcImPaint.h"
#include "KtVector4.h"
#include "imguix.h"
#include "KtuMath.h"
#include <assert.h>
#include "imgui_internal.h"
#include "KtLineS2d.h"
#include "layout/KuLayoutUtil.h"


KcImPaint::KcImPaint(camera_type& cam) : camera_(cam)
{
	coords_.push_back(k_coord_world);
}


void KcImPaint::beginPaint()
{
	camera_.updateProjectMatrixs();
}


void KcImPaint::endPaint()
{
	// depth sorting
}


KcImPaint::rect_t KcImPaint::viewport() const
{
	return camera_.viewport();
}


void KcImPaint::setViewport(const rect_t& vp)
{
	camera_.setViewport(vp);
}


void KcImPaint::pushClipRect(const rect_t& cr)
{
	ImGui::GetWindowDrawList()->PushClipRect(
		ImVec2(cr.lower().x(), cr.lower().y()), ImVec2(cr.upper().x(), cr.upper().y()));
}


void KcImPaint::popClipRect()
{
	ImGui::GetWindowDrawList()->PopClipRect();
}


void KcImPaint::pushLocal(const mat4& mat)
{
	camera_.pushLocal(mat);
}


void KcImPaint::popLocal()
{
	camera_.popLocal();
}


void KcImPaint::pushCoord(KeCoordType type)
{
	coords_.push_back(type);
}


void KcImPaint::popCoord()
{
	assert(coords_.size() > 1);
	coords_.pop_back();
}


KcImPaint::KeCoordType KcImPaint::currentCoord() const
{
	return KeCoordType(coords_.back());
}


KcImPaint::point4 KcImPaint::project(const point4& pt) const
{
	switch (coords_.back())
	{
	case k_coord_world:
		return camera_.localToScreen(pt);

	case k_coord_screen:
		return camera_.localToWorld(pt); // 仅执行局部变换

	default:
		break;
	}

	assert(false);
	return pt;
}


KcImPaint::point4 KcImPaint::unproject(const point4& pt) const
{
	switch (coords_.back())
	{
	case k_coord_world:
		return camera_.screenToLocal(pt);

	case k_coord_screen:
		return camera_.worldToLocal(pt); // 仅执行局部变换

	default:
		break;
	}

	assert(false);
	return pt;
}


KcImPaint::point4 KcImPaint::localToWorld(const point4& pt) const
{
	return camera_.localToWorld(pt);
}


void KcImPaint::setColor(const color_t& clr)
{
	clr_ = clr;
}


void KcImPaint::setPointSize(double size)
{
	pointSize_ = size;
}


void KcImPaint::setLineWidth(double width)
{
	lineWidth_ = width;
}


void KcImPaint::setLineStyle(int style)
{
	lineStyle_ = style;
}


void KcImPaint::drawPoint(const point3& pos)
{
	auto drawList = ImGui::GetWindowDrawList();
	drawList->AddCircleFilled(project_(pos), pointSize_ * 0.5, color_());
}


void KcImPaint::drawLine_(const ImVec2& pt0, const ImVec2& pt1)
{
	if (lineStyle_ == KpPen::k_solid)
		ImGui::GetWindowDrawList()->AddLine(pt0, pt1, color_(), lineWidth_);
	else if (lineStyle_ == KpPen::k_dash)
		drawLinePattern_(pt0, pt1, { 6, 6 });
	else if (lineStyle_ == KpPen::k_dot)
		drawLinePattern_(pt0, pt1, { 2, 6 });
}


void KcImPaint::drawLine(const point3& from, const point3& to)
{
	drawLine_(project_(from, true), project_(to, true));
}


void KcImPaint::drawLineStrip(point_getter fn, unsigned count)
{
	auto drawList = ImGui::GetWindowDrawList();

	if (lineStyle_ == KpPen::k_solid) {
		// TODO: 点数太大（~44k）时会crack
		// 去除drawList->Flags的ImDrawListFlags_AntiAliasedLines标记可正常运行，但速度很慢
		for (unsigned i = 0; i < count; i++)
			drawList->PathLineTo(project_(fn(i), true));
		drawList->PathStroke(color_(), 0, lineWidth_); 
	}
	else {
		ImVec2 pt0 = project_(fn(0), true);
		for (unsigned i = 1; i < count; i++) {
			auto pt1 = project_(fn(i), true);
			drawLine_(pt0, pt1);
			pt1 = pt0;
		}
	}
}


void KcImPaint::drawRect(const point3& lower, const point3& upper)
{
	auto drawList = ImGui::GetWindowDrawList();
	drawList->AddRect(project_(lower, true), project_(upper, true), color_());
}


void KcImPaint::fillTriangle(point3 pts[3])
{
	auto drawList = ImGui::GetWindowDrawList();
	auto p0 = project_(pts[0]);
	auto p1 = project_(pts[1]);
	auto p2 = project_(pts[2]);
	drawList->AddTriangleFilled(p0, p1, p2, color_());
}


void KcImPaint::fillTriangle(point3 pts[3], color_t clrs[3])
{
	auto drawList = ImGui::GetWindowDrawList();
	drawList->PrimReserve(3, 3);

	auto uv = drawList->_Data->TexUvWhitePixel;

	auto p0 = project_(pts[0]);
	auto p1 = project_(pts[1]);
	auto p2 = project_(pts[2]);

	auto vtxIdx0 = drawList->_VtxCurrentIdx;
	drawList->PrimVtx(p0, uv, imColor(clrs[0]));
	drawList->PrimVtx(p1, uv, imColor(clrs[1]));
	drawList->PrimVtx(p2, uv, imColor(clrs[2]));
}


void KcImPaint::fillRect(const point3& lower, const point3& upper)
{
	auto drawList = ImGui::GetWindowDrawList();
	drawList->AddRectFilled(project_(lower), project_(upper), color_());
}


void KcImPaint::fillQuad(point3 pts[4])
{
	auto drawList = ImGui::GetWindowDrawList();
	ImVec2 vec[4];
	for (int i = 0; i < 4; i++)
		vec[i] = project_(pts[i]);
	drawList->AddQuadFilled(vec[0], vec[1], vec[2], vec[3], color_());
}


void KcImPaint::fillConvexPoly(point_getter fn, unsigned count)
{
	auto drawList = ImGui::GetWindowDrawList();

	drawList->_Path.resize(count);

	for (unsigned i = 0; i < count; i++)
		drawList->_Path[i] = project_(fn(i));

	drawList->PathFillConvex(color_());
}


// 基于imgui的优化实现
void KcImPaint::fillBetween(point_getter fn1, point_getter fn2, unsigned count)
{
	auto drawList = ImGui::GetWindowDrawList();
	int vxt_count = count * 2 + count - 1; // 最多可能有count-1个交点
	int idx_count = (count - 1) * 6; // 每个区间绘制2个三角形，共6个索引
	drawList->PrimReserve(idx_count, vxt_count);

	auto uv = drawList->_Data->TexUvWhitePixel;
	auto clr = color_();

	auto p00 = project_(fn1(0));
	auto p01 = project_(fn2(0));

	auto vtxIdx0 = drawList->_VtxCurrentIdx;
	drawList->PrimWriteVtx(p00, uv, clr);
	drawList->PrimWriteVtx(p01, uv, clr);

	int noninters(0); // 统计不相交的次数

	for (unsigned i = 1; i < count; i++) {
		auto p10 = project_(fn1(i));
		auto p11 = project_(fn2(i));

		auto vtxIdx1 = drawList->_VtxCurrentIdx;
		drawList->PrimWriteVtx(p10, uv, clr);
		drawList->PrimWriteVtx(p11, uv, clr);

		using point2 = KtPoint<float_t, 2>;
		KtLineS2d<float_t> ln0((const point2&)p00, (const point2&)p10);
		KtLineS2d<float_t> ln1((const point2&)p01, (const point2&)p11);
		auto pt = ln0.intersects(ln1);
		if (pt) { // 相交
			drawList->PrimWriteVtx((const ImVec2&)pt.value(), uv, clr);

			drawList->PrimWriteIdx(vtxIdx0 + 1);
			drawList->PrimWriteIdx(vtxIdx0);
			drawList->PrimWriteIdx(vtxIdx1 + 2);

			drawList->PrimWriteIdx(vtxIdx1);
			drawList->PrimWriteIdx(vtxIdx1 + 1);
			drawList->PrimWriteIdx(vtxIdx1 + 2);
		}
		else { // 不相交
			drawList->PrimWriteIdx(vtxIdx0 + 1);
			drawList->PrimWriteIdx(vtxIdx0);
			drawList->PrimWriteIdx(vtxIdx1);

			drawList->PrimWriteIdx(vtxIdx1);
			drawList->PrimWriteIdx(vtxIdx1 + 1);
			drawList->PrimWriteIdx(vtxIdx0 + 1);

			++noninters;
		}

		vtxIdx0 = vtxIdx1;
		p00 = p10, p01 = p11;
	}

	drawList->PrimUnreserve(0, noninters);
}


void KcImPaint::drawGeom(geom_ptr geom)
{
	auto drawList = ImGui::GetWindowDrawList();
	drawList->PrimReserve(geom->indexCount(), geom->vertexCount());
	auto uv = drawList->_Data->TexUvWhitePixel;
	auto clr = color_();

	auto vtxIdx0 = drawList->_VtxCurrentIdx;

	for (unsigned i = 0; i < geom->vertexCount(); i++) {
		auto& vtx = geom->vertexAt(i);
		drawList->PrimWriteVtx(project_(vtx), uv, clr);
	}

	for (unsigned i = 0; i < geom->indexCount(); i++)
		drawList->PrimWriteIdx(geom->indexAt(i) + vtxIdx0);
}


void KcImPaint::drawText(const point3& anchor, const char* text, int align)
{
	auto drawList = ImGui::GetWindowDrawList();

	auto ap = projectp(anchor);
	auto r = KuLayoutUtil::anchorAlignedRect({ ap.x(), ap.y() }, textSize(text), align);
	
	drawList->AddText(ImVec2(r.lower().x(), r.lower().y()), color_(), text);
}


KcImPaint::point2 KcImPaint::textSize(const char* text) const
{
	auto sz = ImGui::CalcTextSize(text);
	return { sz.x, sz.y };
}


ImVec2 KcImPaint::project_(const point3& pt, bool round) const
{
	auto pos = projectp(pt);
	return round ? ImVec2(int(pos.x()), int(pos.y())) : ImVec2(pos.x(), pos.y());
}


void KcImPaint::drawLinePattern_(const ImVec2& pt0, const ImVec2& pt1, const std::vector<int>& pat)
{
	assert(pat.size() % 2 == 0);

	if (pt0.x == pt1.x && pt0.y == pt1.y)
		return;

	if (pt1.x < pt0.x) {
		drawLinePattern_(pt1, pt0, pat);
		return;
	}

	auto drawList = ImGui::GetWindowDrawList();
	auto xlen = pt1.x - pt0.x;
	auto ylen = pt1.y - pt0.y;
	auto pt = pt0;
	if (xlen >= std::abs(ylen)) {
		assert(xlen > 0);
		auto slope = ylen / xlen;

		while (pt.x < pt1.x) {
			for (unsigned i = 0; i < pat.size(); i+=2) {
				auto xto = pt.x + pat[i];
				if (xto > pt1.x)
					xto = pt1.x;
				auto yto = pt.y + (xto - pt.x) * slope;

				drawList->AddLine({ std::round(pt.x), std::round(pt.y) }, 
					{ std::round(xto), std::round(yto) }, color_());

				pt.x = xto + pat[i + 1];
				pt.y = yto + pat[i + 1] * slope;
			}
		}
	}
	else {
		auto slope = xlen / ylen;

		if (ylen > 0) {
			while (pt.y < pt1.y) {
				for (unsigned i = 0; i < pat.size(); i += 2) {
					auto yto = pt.y + pat[i];
					if (yto > pt1.y)
						yto = pt1.y;
					auto xto = pt.x + (yto - pt.y) * slope;

					drawList->AddLine({ std::round(pt.x), std::round(pt.y) }, 
						{ std::round(xto), std::round(yto) }, color_());

					pt.y = yto + pat[i + 1];
					pt.x = xto + pat[i + 1] * slope;
				}
			}
		}
		else {
			while (pt.y > pt1.y) {
				for (unsigned i = 0; i < pat.size(); i += 2) {
					auto yto = pt.y - pat[i];
					if (yto < pt1.y)
						yto = pt1.y;
					auto xto = pt.x + (yto - pt.y) * slope;

					drawList->AddLine({ std::round(pt.x), std::round(pt.y) }, 
						{ std::round(xto), std::round(yto) }, color_());

					pt.y = yto - pat[i + 1];
					pt.x = xto + pat[i + 1] * slope;
				}
			}
		}
	}
}
