#pragma once
#include "plot/KvPaintHelper.h"


class KcCairoPaint : public KvPaintHelper
{
	using super_ = KvPaintHelper;

public:
	KcCairoPaint();
	virtual ~KcCairoPaint();

	void beginPaint() override;
	void endPaint() override;

	void enableClipBox(point3 lower, point3 upper) override {}
	void disableClipBox() override {}

	void enablePolygonOffset(bool b) override{}

	void enableDepthTest(bool b) override{}
	bool depthTest() const override { return false; }

	void enableAntialiasing(bool b) override{}
	bool antialiasing() const override { return false; }

	void enableFlatShading(bool b) override {}
	bool flatShading() const override { return false; }


	void enableLighting(bool b) override {}
	bool lighting() const override { return false; }

	void setRect(const rect_t& rc);

	/* Set surface to translucent color without disturbing graphics state. */
	void clear(const color4f& clr);

	void drawLine(const point3& from, const point3& to) override;

	void drawMarker(const point3& pt) override;

	void* drawMarkers(point_getter fn, color_getter clr, size_getter size, unsigned count) override;

	void* drawLineStrips(const std::vector<point_getter>& fns, const std::vector<unsigned>& cnts) override;

	void fillTriangle(point3 pts[3]) override;

	void fillTriangle(point3 pts[3], color_t clrs[3]) override;

	void fillRect(const point3& lower, const point3& upper) override;

	void fillConvexPoly(point_getter fn, unsigned count) override;

	void* fillBetween(point_getter line1, point_getter line2, unsigned count) override;

	void drawText(const point3& topLeft, const point3& hDir, const point3& vDir, const char* text) override;

	void drawText(const point3& anchor, const char* text, int align) override;

	void* drawTexts(const std::vector<point3>& anchors, const std::vector<std::string>& texts, int align, const point2f& spacing) override;

	void* drawGeom(vtx_decl_ptr decl, geom_ptr geom) override;

	void grab(int x, int y, int width, int height, void* data) override;

	point2 textSize(const char* text) const override;

protected:
	void destroy_();

	void applyLineCxt_();

protected:
	void* cxt_{ nullptr }; // the cr of cairo
	void* surf_{ nullptr }; // the surface of cairo
	rect_t canvas_;
};
