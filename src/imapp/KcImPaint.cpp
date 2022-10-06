#include "KcImPaint.h"
#include "KtVector4.h"


KcImPaint::KcImPaint(camera_type& cam) : camera_(cam)
{
}


void KcImPaint::setColor(const color_t& clr)
{
	clr_ = clr;
}


void KcImPaint::setLineWidth(double width)
{
	lineWidth_ = static_cast<decltype(lineWidth_)>(width);
}


void KcImPaint::drawLine(const point3& from, const point3& to)
{
	auto drawList = ImGui::GetWindowDrawList();
	drawList->AddLine(world2Pos_(from), world2Pos_(to), color(), lineWidth_);
}


void KcImPaint::drawText(const point3& anchor, const char* text, int align)
{
	auto drawList = ImGui::GetWindowDrawList();
	// TODO: 根据align调整anchor
	drawList->AddText(world2Pos_(anchor), color(), text);
}


ImVec2 KcImPaint::world2Pos_(const point3& pt) const
{
	auto pos = camera_.worldToScreen(pt);
	return { pos.x(), pos.y() };
}
