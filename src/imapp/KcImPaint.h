#pragma once
#include "plot/KvPaint.h"
#include "KtProjector3d.h"
#include "imgui.h"


class KcImPaint : public KvPaint
{
public:
	using camera_type = KtProjector<float_t>;

	KcImPaint(camera_type& cam);

	void beginPaint() override;
	void endPaint() override;

	rect_t viewport() const override;
	void setViewport(const rect_t& vp) override;

	void pushClipRect(const rect_t& cr) override;
	void popClipRect() override;

	void pushLocal(const mat4& mat) override;
	void popLocal() override;

	void pushCoord(KeCoordType type) override;
	void popCoord() override;
	KeCoordType currentCoord() const override;

	point4 project(const point4& worldPt) const override;

	point4 unproject(const point4& pt) const override;

	point4 localToWorld(const point4& pt) const override;

	point4 worldToLocal(const point4& pt) const override;

	void setColor(const color_t& clr) override;

	void setPointSize(float_t size) override;

	void setLineWidth(float_t width) override;

	void setLineStyle(int style) override;

	void drawPoint(const point3& pos) override;

	void drawLine(const point3& from, const point3& to) override;

	void drawLineStrip(point_getter fn, unsigned count) override;

	void drawRect(const point3& lower, const point3& upper) override;

	void fillTriangle(point3 pts[3]) override;

	void fillTriangle(point3 pts[3], color_t clrs[3]) override;

	void fillRect(const point3& lower, const point3& upper) override;

	void fillQuad(point3 pts[4]) override;

	void fillConvexPoly(point_getter fn, unsigned count) override;

	void fillBetween(point_getter line1, point_getter line2, unsigned count) override;

	void drawText(const point3& anchor, const char* text, int align) override;

	//void drawGeom(vtx_decl_ptr decl, geom_ptr geom) override;

	point2 textSize(const char* text) const override;

protected:
	ImVec2 project_(const point3& pt, bool round = false) const;

	static ImColor imColor(const color_t& clr) {
		return ImColor(clr.r(), clr.g(), clr.b(), clr.a());
	}

	ImColor color_() const { return imColor(clr_); }

	void drawLine_(const ImVec2& from, const ImVec2& to);

	void drawLineDot_(const ImVec2& from, const ImVec2& to);

	// @pat: 直线绘制模板，长度为双数，格式为：绘制长度, 留白长度, 绘制长度, 留白长度...
	void drawLinePattern_(const ImVec2& from, const ImVec2& to, const std::vector<int>& pat);


protected:
	camera_type& camera_;
	color_t clr_{ 0, 0, 0, 1 };
	float_t lineWidth_{ 1 };
	int lineStyle_{ 0 };
	float_t pointSize_{ 1 };

	std::vector<int> coords_; // 坐标系堆栈
};
