#pragma once
#include "plot/KvPaint.h"
#include "KtCamera.h"
#include "imgui.h"


class KcImPaint : public KvPaint
{
	using camera_type = KtCamera<float>;
	
public:

	KcImPaint(camera_type& cam);

	void setColor(const color_t& clr) override;

	void setLineWidth(double width) override;

	void drawLine(const point3& from, const point3& to) override;

	void drawText(const point3& anchor, const char* text, int align) override;

private:
	ImVec2 world2Pos_(const point3& pt) const;

	ImColor color_() const {
		return ImColor(clr_.r(), clr_.g(), clr_.b(), clr_.a());
	}

private:
	camera_type& camera_;
	color_t clr_{ 0, 0, 0, 0 };
	float lineWidth_{ 1 };
};
