#include "KcImPaint.h"
#include "KtVector4.h"


KcImPaint::KcImPaint(camera_type& cam) : camera_(cam)
{
}


void KcImPaint::setColor(const color_t& clr)
{
	clr_ = clr;
}


void KcImPaint::setPointSize(float size)
{
	pointSize_ = size;
}


void KcImPaint::setLineWidth(float width)
{
	lineWidth_ = width;
}


void KcImPaint::drawPoint(const point3& pos)
{
	auto drawList = ImGui::GetWindowDrawList();
	drawList->AddCircleFilled(world2Pos_(pos), pointSize_, color_());
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
	// TODO: 根据align调整anchor
	drawList->AddText(world2Pos_(anchor), color_(), text);
}


ImVec2 KcImPaint::world2Pos_(const point3& pt) const
{
	auto pos = camera_.worldToScreen(pt);
	return { pos.x(), pos.y() };
}
