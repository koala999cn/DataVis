#pragma once
#include "plot/KvPaint.h"
#include "KtCamera.h"
#include "imgui.h"


class KcImPaint : public KvPaint
{
	using camera_type = KtCamera<float_t>;
	
public:

	KcImPaint(camera_type& cam);

	rect viewport() const override;
	void setViewport(const rect& vp) override;

	point2 project(const point3& worldPt) const override;

	void setColor(const color_t& clr) override;

	void setPointSize(float_t size) override;

	void setLineWidth(float_t width) override;

	void drawPoint(const point3& pos) override;

	void drawLine(const point3& from, const point3& to) override;

	void drawText(const point3& anchor, const char* text, int align) override;

	point2 textSize(const char* text) const override;

private:
	ImVec2 world2Pos_(const point3& pt) const;

	ImColor color_() const {
		return ImColor(clr_.r(), clr_.g(), clr_.b(), clr_.a());
	}

private:
	camera_type& camera_;
	color_t clr_{ 0, 0, 0, 1 };
	float_t lineWidth_{ 1 };
	float_t pointSize_{ 2 };
};
