#pragma once
#include "KtColor.h"
#include <functional>
#include "KvRenderable.h"
#include "KpContext.h"


enum KeAlignment
{
	k_align_center = 0x00,
	k_align_left = 0x01,
	k_align_right = 0x02,
	k_align_top = 0x04,
	k_align_bottom = 0x08,

	// 当位于边框的外侧对齐时，使用以下2个枚举量区分水平和纵向优先级
	// 例如，如果k_align_left和k_align_top均被设置，则
	//   -- 当k_align_vert_first有效时，位于边框的顶端位置靠左对齐；
	//   -- 当k_align_horz_first有效时，位于边框的左端位置靠上对齐。
	k_align_vert_first = 0x40,
	k_align_horz_first = 0x80
};

// 一个简单的3d绘制接口

class KvPaint
{
public:
	using color_t = color4f;
	using float_t = typename KvRenderable::float_t;
	using point2 = KtPoint<float_t, 2>;
	using point3 = KtPoint<float_t, 3>;
	using rect = KtAABB<float_t, 2>;
	using point_getter = std::function<point3(unsigned)>;

	virtual rect viewport() const = 0;
	virtual void setViewport(const rect& vp) = 0;

	// project world point to screen point
	virtual point2 project(const point3& worldPt) const = 0;

	virtual void setColor(const color_t& clr) = 0;

	virtual void setPointSize(double size) = 0;

	virtual void setLineWidth(double width) = 0;

	virtual void setLineStyle(int style) = 0;

	virtual void drawPoint(const point3& pt) = 0;

	virtual void drawPoints(const point3 pts[], unsigned count);

	void drawPoints(point_getter fn, unsigned count);

	virtual void drawLine(const point3& from, const point3& to) = 0;

	virtual void drawLineStrip(const point3 pts[], unsigned count);

	void drawLineStrip(point_getter fn, unsigned count);

	virtual void drawLineLoop(const point3 pts[], unsigned count);

	void drawLineLoop(point_getter fn, unsigned count);

	virtual void fillRect(const point3& lower, const point3& upper) = 0;

	virtual void fillQuad(const point3& pt0, const point3& pt1, const point3& pt2, const point3& pt3) = 0;

	virtual void drawText(const point3& anchor, const char* text, int align) = 0;

	// 一些尺寸计算函数

	virtual point2 textSize(const char* text) const = 0;

	// 一些便捷函数

	// 以pos为锚点，以align为对齐方式，计算绘制text的矩形空间
	// 所用参数均为屏幕空间坐标
	rect textRect(const point2& pos, const char* text, int align) const;

	void apply(const KpPen& cxt);

	void apply(const KpBrush& cxt);

	void apply(const KpFont& cxt);
};
