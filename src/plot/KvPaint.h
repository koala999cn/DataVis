#pragma once
#include "KtColor.h"
#include <functional>
#include "KvRenderable.h"
#include "KpContext.h"
#include "KtGeometry.h"
#include "KtMatrix4.h"
#include "KtQuaternion.h"


// 一个简单的3d绘制接口

class KvPaint
{
public:
	using color_t = color4f;
	using float_t = typename KvRenderable::float_t;
	using rect_t = KtAABB<float_t, 2>;
	using point2 = KtPoint<float_t, 2>;
	using point3 = KtPoint<float_t, 3>;
	using point4 = KtPoint<float_t, 4>;
	using mat4 = KtMatrix4<float_t>;
	using point_getter = std::function<point3(unsigned)>;
	using geom_ptr = std::shared_ptr<KtGeometry<float_t, unsigned>>;

	virtual void beginPaint() = 0;
	virtual void endPaint() = 0;

	virtual rect_t viewport() const = 0;
	virtual void setViewport(const rect_t& vp) = 0;

	virtual void pushClipRect(const rect_t& cr) = 0;
	virtual void popClipRect() = 0;

	// 改变model变换矩阵，即局部坐标系
	virtual void pushLocal(const mat4& mat) = 0;
	virtual void popLocal() = 0;

	enum KeCoordType
	{
		k_coord_world, // 世界坐标系，即绘图坐标系
		k_coord_screen, // 屏幕坐标系
		k_coord_viewport // 各轴规范化为[0, 1]的标准坐标系
	};

	// 改变当前坐标系类型
	virtual void pushCoord(KeCoordType type) = 0;
	virtual void popCoord() = 0;

	// project world point to screen point
	virtual point4 project(const point4& pt) const = 0;

	virtual void setColor(const color_t& clr) = 0;

	virtual void setPointSize(double size) = 0;

	virtual void setLineWidth(double width) = 0;

	virtual void setLineStyle(int style) = 0;

	virtual void drawPoint(const point3& pt) = 0;

	virtual void drawPoints(const point3 pts[], unsigned count);

	virtual void drawPoints(point_getter fn, unsigned count);

	virtual void drawLine(const point3& from, const point3& to) = 0;

	virtual void drawLineStrip(const point3 pts[], unsigned count);

	virtual void drawLineStrip(point_getter fn, unsigned count);

	virtual void drawLineLoop(const point3 pts[], unsigned count);

	virtual void drawLineLoop(point_getter fn, unsigned count);

	virtual void drawRect(const point3& lower, const point3& upper);

	virtual void fillTriangle(point3 pts[3]) = 0;

	virtual void fillTriangle(point3 pts[3], color_t clrs[3]) = 0;

	virtual void fillRect(const point3& lower, const point3& upper) = 0;

	virtual void fillQuad(point3 pts[4]);

	virtual void fillQuad(point3 pts[4], color_t clrs[4]);

	virtual void fillConvexPoly(point_getter fn, unsigned count) = 0;

	virtual void fillBetween(point_getter line1, point_getter line2, unsigned count) = 0;

	virtual void drawText(const point3& anchor, const char* text, int align) = 0;

	virtual void drawGeom(geom_ptr geom) = 0;


	// 一些尺寸计算函数

	virtual point2 textSize(const char* text) const = 0;

	// 一些便捷函数

	// 点投影
	point3 projectp(const point3& pt) const {
		auto r = project(point4(pt.x(), pt.y(), pt.z(), 1));
		return { r.x(), r.y(), r.z() };
	}

	// 矢量投影
	point3 projectv(const point3& v) const {
		auto r = project(point4(v.x(), v.y(), v.z(), 0));
		return { r.x(), r.y(), r.z() };
	}

	void drawRect(const rect_t& rc) {
		drawRect(point3(rc.lower().x(), rc.lower().y(), 0),
			     point3(rc.upper().x(), rc.upper().y(), 0));
	}

	void drawRect(const point2& lower, const point2& upper) {
		drawRect(point3(lower.x(), lower.y(), 0), point3(upper.x(), upper.y(), 0));
	}

	void fillRect(const rect_t& rc) {
		fillRect(point3(rc.lower().x(), rc.lower().y(), 0),
			point3(rc.upper().x(), rc.upper().y(), 0));
	}

	void fillRect(const point2& lower, const point2& upper) {
		fillRect(point3(lower.x(), lower.y(), 0), point3(upper.x(), upper.y(), 0));
	}

	void fillQuad(point2 pts[4]) {
		point3 pt3s[4];
		for (int i = 0; i < 4; i++)
			pt3s[i] = { pts[i].x(), pts[i].y(), 0 };
		fillQuad(pt3s);
	}

	void fillQuad(point2 pts[4], color_t clrs[4]) {
		point3 pt3s[4];
		for (int i = 0; i < 4; i++)
			pt3s[i] = { pts[i].x(), pts[i].y(), 0 };
		fillQuad(pt3s, clrs);
	}

	void drawText(const point2& ach, const char* text, int align) {
		drawText(point3(ach.x(), ach.y(), 0), text, align);
	}

	// 以pos为锚点，以align为对齐方式，计算绘制text的矩形空间
	// 所用参数均为屏幕空间坐标
	rect_t textRect(const point2& pos, const char* text, int align) const;

	void apply(const KpPen& cxt);

	void apply(const KpBrush& cxt);

	void apply(const KpFont& cxt);
};
