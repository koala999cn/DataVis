#include "KcImPaint.h"
#include "KtVector4.h"
#include "imguix.h"
#include "KtuMath.h"
#include <assert.h>


KcImPaint::KcImPaint(camera_type& cam) : camera_(cam)
{
}


KcImPaint::rect KcImPaint::viewport() const
{
	return camera_.viewport();
}


void KcImPaint::setViewport(const rect& vp)
{
	camera_.viewport() = vp;
}


KcImPaint::point2 KcImPaint::project(const point3& worldPt) const
{
	return camera_.worldToScreen(worldPt);
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


void KcImPaint::drawLine(const point3& from, const point3& to)
{
	// TODO: ²Ã¼ô
	auto drawList = ImGui::GetWindowDrawList();
	auto pt0 = world2Pos_(from, true), pt1 = world2Pos_(to, true);		

	if (lineStyle_ == KpPen::k_solid)
		drawList->AddLine(pt0, pt1, color_(), lineWidth_);
	else if (lineStyle_ == KpPen::k_dash)
		drawLinePattern_(pt0, pt1, { 6, 6 });
	else if (lineStyle_ == KpPen::k_dot)
		drawLinePattern_(pt0, pt1, { 2, 6 });
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
	if (round) {
		pos.x() = std::round(pos.x());
		pos.y() = std::round(pos.y());
	}
	return ImVec2(pos.x(), pos.y());
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
