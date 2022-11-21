#pragma once
#include "plot/KvPaint.h"
#include "KtProjector.h"
#include "imgui.h"


class KcImPaint : public KvPaint
{
	using camera_type = KtProjector<float_t>;
	
public:

	KcImPaint(camera_type& cam);

	void beginPaint() override;
	void endPaint() override;

	rect viewport() const override;
	void setViewport(const rect& vp) override;

	void pushClipRect(const rect& cr) const override;
	void popClipRect() override;

	point2 project(const point3& worldPt) const override;

	void setColor(const color_t& clr) override;

	void setPointSize(float_t size) override;

	void setLineWidth(float_t width) override;

	void setLineStyle(int style) override;

	void drawPoint(const point3& pos) override;

	void drawLine(const point3& from, const point3& to) override;

	void drawLineStrip(point_getter fn, unsigned count) override;

	void fillRect(const point3& lower, const point3& upper) override;

	void fillQuad(const point3& pt0, const point3& pt1, const point3& pt2, const point3& pt3) override;

	void fillConvexPoly(point_getter fn, unsigned count) override;

	void fillBetween(point_getter line1, point_getter line2, unsigned count) override;

	void drawText(const point3& anchor, const char* text, int align) override;

	void drawGeom(geom_ptr geom) override;

	point2 textSize(const char* text) const override;

private:
	ImVec2 world2Pos_(const point3& pt, bool round = false) const;

	ImColor color_() const {
		return ImColor(clr_.r(), clr_.g(), clr_.b(), clr_.a());
	}

	void drawLine_(const ImVec2& from, const ImVec2& to);

	void drawLineDot_(const ImVec2& from, const ImVec2& to);

	// @pat: 直线绘制模板，长度为双数，格式为：绘制长度, 留白长度, 绘制长度, 留白长度...
	void drawLinePattern_(const ImVec2& from, const ImVec2& to, const std::vector<int>& pat);


private:
	camera_type& camera_;
	color_t clr_{ 0, 0, 0, 1 };
	float_t lineWidth_{ 1 };
	int lineStyle_{ 0 };
	float_t pointSize_{ 2 };

	KtMatrix4<float_t> vp_; // 坐标变换矩阵
};
