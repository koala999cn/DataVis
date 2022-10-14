#include "KcImPaint.h"
#include "KtVector4.h"


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


void KcImPaint::drawPoint(const point3& pos)
{
	auto drawList = ImGui::GetWindowDrawList();
	drawList->AddCircleFilled(world2Pos_(pos), pointSize_ * 0.5, color_());
}


void KcImPaint::drawLine(const point3& from, const point3& to)
{
	// TODO: 裁剪
	auto drawList = ImGui::GetWindowDrawList();
	drawList->AddLine(world2Pos_(from), world2Pos_(to), color_(), lineWidth_);
}


void KcImPaint::drawText(const point3& anchor, const char* text, int align)
{
	auto drawList = ImGui::GetWindowDrawList();

	auto pos = world2Pos_(anchor);

	// 根据align调整pos
	auto szText = ImGui::CalcTextSize(text);

	if (align & k_align_bottom)
		pos.y -= szText.y;
	else if (align & k_align_top)
		pos.y;
	else // k_align_venter
		pos.y -= szText.y * 0.5;

	if (align & k_align_left)
		pos.x;
	else if (align & k_align_right)
		pos.x -= szText.x;
	else // k_align_center
		pos.x -= szText.x * 0.5;


	drawList->AddText(pos, color_(), text);
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
