#pragma once
#include "plot/KvPaintHelper.h"


class KcCairoPaint : public KvPaintHelper
{
	using super_ = KvPaintHelper;

public:
	KcCairoPaint();
	virtual ~KcCairoPaint();

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

	void* drawLineStrips(const std::vector<point_getter>& fns, const std::vector<unsigned>& cnts) override;

	// 重载实现dot & circle的绘制
	void drawMarker(const point3& pt) override;

	void fillTriangle(point3 pts[3]) override;

	void fillTriangle(point3 pts[3], color_t clrs[3]) override;

	void fillRect(const point3& lower, const point3& upper) override;

	void fillPoly(point_getter fn, unsigned count) override;

	void* fillBetween(point_getter line1, point_getter line2, unsigned count) override;

	void drawText(const point3& topLeft, const point3& hDir, const point3& vDir, const std::string_view& text) override;

	void* drawGeom(vtx_decl_ptr decl, geom_ptr geom) override;

	void grab(int x, int y, int width, int height, void* data) override;

	double fontHeight() const;
	double charSpacing() const;

	point2 textSize(const std::string_view& text) const override;

protected:
	void destroy_();

	void applyLineCxt_();

protected:
	void* cxt_{ nullptr }; // the cr of cairo
	void* surf_{ nullptr }; // the surface of cairo
	rect_t canvas_;
};
