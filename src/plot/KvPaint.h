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
	using float_type = typename KvRenderable::float_type;
	using point = KtPoint<float_type, 3>;
	using point_getter = std::function<point(unsigned)>;

	virtual void setColor(const color_t& clr) = 0;

	virtual void setPointSize(double size) = 0;

	virtual void setLineWidth(double width) = 0;

	virtual void drawPoint(const point& pt) = 0;

	virtual void drawPoints(const point pts[], unsigned count);

	void drawPoints(point_getter fn, unsigned count);

	virtual void drawLine(const point& from, const point& to) = 0;

	virtual void drawLineStrip(const point pts[], unsigned count);

	void drawLineStrip(point_getter fn, unsigned count);

	virtual void drawLineLoop(const point pts[], unsigned count);

	void drawLineLoop(point_getter fn, unsigned count);

	virtual void drawText(const point& anchor, const char* text, int align) = 0;


	// 一些便捷函数

	void apply(const KpLineContext& cxt);

	void apply(const KpScatterContext& cxt);

private:

};
