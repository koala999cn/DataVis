#pragma once
#include "plot/KvPaintHelper.h"
#include "imgui.h"

class KcImPaint : public KvPaintHelper
{
public:

	void pushClipRect(const rect_t& cr) override;
	void popClipRect() override;

	void drawMarker(const point3& pos) override;

	void drawLine(const point3& from, const point3& to) override;

	void* drawLineStrip(point_getter fn, unsigned count) override;

	void drawRect(const point3& lower, const point3& upper) override;

	void fillTriangle(point3 pts[3]) override;

	void fillTriangle(point3 pts[3], color_t clrs[3]) override;

	void fillRect(const point3& lower, const point3& upper) override;

	void fillQuad(point3 pts[4]) override;

	void fillConvexPoly(point_getter fn, unsigned count) override;

	void* fillBetween(point_getter line1, point_getter line2, unsigned count) override;

	void drawText(const point3& anchor, const char* text, int align) override;

	//void drawGeom(vtx_decl_ptr decl, geom_ptr geom) override;

	point2 textSize(const char* text) const override;

protected:
	ImVec2 project_(const point3& pt, bool round = false) const;

	static ImColor imColor(const color_t& clr) {
		return ImColor(clr.r(), clr.g(), clr.b(), clr.a());
	}

	ImColor color_() const { return imColor(clr_); }
	ImColor secondaryColor_() const { return imColor(secondaryClr_); }

	void drawLine_(const ImVec2& from, const ImVec2& to);

	void drawLineDot_(const ImVec2& from, const ImVec2& to);

	// @pat: 直线绘制模板，长度为双数，格式为：绘制长度, 留白长度, 绘制长度, 留白长度...
	void drawLinePattern_(const ImVec2& from, const ImVec2& to, const std::vector<int>& pat);

	void addTriMarker_(const ImVec2& center, const std::array<float, 2> pts[], bool outline);
};
