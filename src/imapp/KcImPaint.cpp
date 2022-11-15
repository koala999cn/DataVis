#include "KcImPaint.h"
#include "KtVector4.h"
#include "imguix.h"
#include "KtuMath.h"
#include <assert.h>
#include "imgui_internal.h"
#include "KtLineS2d.h"


KcImPaint::KcImPaint(camera_type& cam) : camera_(cam)
{
}


void KcImPaint::beginPaint()
{
	camera_.updateProjectMatrixs();
}


void KcImPaint::endPaint()
{
	// depth sorting
}


KcImPaint::rect KcImPaint::viewport() const
{
	return camera_.viewport();
}


void KcImPaint::setViewport(const rect& vp)
{
	camera_.setViewport(vp);
}


void KcImPaint::pushClipRect(const rect& cr) const
{
	ImGui::GetWindowDrawList()->PushClipRect(
		ImVec2(cr.lower().x(), cr.lower().y()), ImVec2(cr.upper().x(), cr.upper().y()));
}


void KcImPaint::popClipRect()
{
	ImGui::GetWindowDrawList()->PopClipRect();
}


KcImPaint::point2 KcImPaint::project(const point3& worldPt) const
{
	auto scrPt = camera_.worldToScreen(worldPt);
	return { scrPt.x(), scrPt.y() };
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
	drawList->AddCircleFilled(world2Pos_(pos), pointSize_ * 0.5, color_());
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
	drawLine_(world2Pos_(from, true), world2Pos_(to, true));
}


void KcImPaint::drawLineStrip(point_getter fn, unsigned count)
{
	auto drawList = ImGui::GetWindowDrawList();

	if (lineStyle_ == KpPen::k_solid) {
		// TODO: 点数太大（~44k）时会crack
		// 去除drawList->Flags的ImDrawListFlags_AntiAliasedLines标记可正常运行，但速度很慢
		for (unsigned i = 0; i < count; i++)
			drawList->PathLineTo(world2Pos_(fn(i), true));
		drawList->PathStroke(color_(), 0, lineWidth_); 
	}
	else {
		ImVec2 pt0 = world2Pos_(fn(0), true);
		for (unsigned i = 1; i < count; i++) {
			auto pt1 = world2Pos_(fn(i), true);
			drawLine_(pt0, pt1);
			pt1 = pt0;
		}
	}
}


void KcImPaint::fillRect(const point3& lower, const point3& upper)
{
	auto drawList = ImGui::GetWindowDrawList();
	drawList->AddRectFilled(world2Pos_(lower), world2Pos_(upper), color_());
}


void KcImPaint::fillQuad(const point3& pt0, const point3& pt1, const point3& pt2, const point3& pt3)
{
	auto drawList = ImGui::GetWindowDrawList();
	drawList->AddQuadFilled(world2Pos_(pt0), world2Pos_(pt1), world2Pos_(pt2), world2Pos_(pt3), color_());
}


void KcImPaint::fillConvexPoly(point_getter fn, unsigned count)
{
	auto drawList = ImGui::GetWindowDrawList();

	drawList->_Path.resize(count);

	for (unsigned i = 0; i < count; i++)
		drawList->_Path[i] = world2Pos_(fn(i));

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

	auto p00 = world2Pos_(fn1(0));
	auto p01 = world2Pos_(fn2(0));

	auto vtxIdx0 = drawList->_VtxCurrentIdx;
	drawList->PrimWriteVtx(p00, uv, clr);
	drawList->PrimWriteVtx(p01, uv, clr);

	int noninters(0); // 统计不相交的次数

	for (unsigned i = 1; i < count; i++) {
		auto p10 = world2Pos_(fn1(i));
		auto p11 = world2Pos_(fn2(i));

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
		drawList->PrimWriteVtx(world2Pos_(vtx), uv, clr);
	}

	for (unsigned i = 0; i < geom->indexCount(); i++)
		drawList->PrimWriteIdx(geom->indexAt(i) + vtxIdx0);
}


void KcImPaint::drawText(const point3& anchor, const char* text, int align)
{
	auto drawList = ImGui::GetWindowDrawList();

	auto r = textRect(project(anchor), text, align);

	drawList->AddText(ImVec2(r.lower().x(), r.lower().y()), color_(), text);
}


KcImPaint::point2 KcImPaint::textSize(const char* text) const
{
	auto sz = ImGui::CalcTextSize(text);
	return { sz.x, sz.y };
}


ImVec2 KcImPaint::world2Pos_(const point3& pt, bool round) const
{
	auto pos = camera_.worldToScreen(pt);
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
