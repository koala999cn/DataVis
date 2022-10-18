#include "KcImPaint.h"
#include "KtVector4.h"
#include "imguix.h"
#include "KtuMath.h"


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
	auto pt0 = world2Pos_(from), pt1 = world2Pos_(to);		
	round_(pt0); round_(pt1);

	if (lineStyle_ == KpPen::k_solid) 
		drawList->AddLine(pt0, pt1, color_(), lineWidth_);
	else if (lineStyle_ == KpPen::k_dash)
		ImGuiX::AddLineDashed(drawList, pt0, pt1, color_(), lineWidth_, 100);
	else if (lineStyle_ == KpPen::k_dot)
		drawLineDot_(pt0, pt1);
}


void KcImPaint::fillRect(const point3& lower, const point3& upper)
{
	auto drawList = ImGui::GetWindowDrawList();
	drawList->AddRectFilled(world2Pos_(lower), world2Pos_(upper), color_());
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


ImVec2 KcImPaint::world2Pos_(const point3& pt) const
{
	auto pos = camera_.worldToScreen(pt);
	return ImVec2(pos.x(), pos.y());
}


void KcImPaint::drawLineDot_(const ImVec2& pt0, const ImVec2& pt1)
{
	auto drawList = ImGui::GetWindowDrawList();
	auto xlen = pt1.x - pt0.x;
	auto ylen = pt1.y - pt0.y;
	auto pt = pt0;
	if (std::abs(xlen) >= std::abs(ylen)) {
		auto slope = ylen / xlen;
		auto dx = 6;
		auto dy = dx * slope;
		if (xlen > 0) {
			while (pt.x < pt1.x) {
				round_(pt);
				drawList->AddCircleFilled(pt, lineWidth_, color_());
				pt.x += dx;
				pt.y += dy;
			}
		}
		else {
			while (pt.x > pt1.x) {
				round_(pt);
				drawList->AddCircleFilled(pt, lineWidth_, color_());
				pt.x -= dx;
				pt.y += dy;
			}
		}
	}
	else {
		auto slope = xlen / ylen;
		auto dy = 6;
		auto dx = dy * slope;
		if (ylen > 0) {
			while (pt.y < pt1.y) {
				round_(pt);
				drawList->AddCircleFilled(pt, lineWidth_, color_());
				pt.y += dy;
				pt.x += dx;
			}
		}
		else {
			while (pt.y > pt1.y) {
				round_(pt);
				drawList->AddCircleFilled(pt, lineWidth_, color_());
				pt.y -= dy;
				pt.x += dx;
			}
		}
	}
}


void KcImPaint::round_(ImVec2& pt)
{
	pt.x = std::round(pt.x);
	pt.y = std::round(pt.y);
}