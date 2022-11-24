#pragma once
#include "KtAABB.h"


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
	k_align_horz_first = 0x80,

	k_align_location_mask = k_align_left | k_align_right | k_align_top | k_align_bottom,
	k_align_side_mask = k_align_vert_first | k_align_horz_first
};

class KuAlignment
{
public:

	using rect = KtAABB<double, 2>;

	static int location(int align);

	static int side(int align);

	static int toggleSide(int align);

	static int toggleHorzFirst(int align);

	static bool sameLocation(int ali1, int ali2);

	static bool sameSide(int ali1, int ali2);

	static bool outside(int align) { return side(align); }

	static bool inside(int align) { return side(align) == 0; }

	// 计算box的lower点位置(屏幕坐标)
	// @align: box的对齐方式
	// @szBox: box的尺寸大小
	// @rc: box拟对齐的rect
	static point2d position(int align, const point2d& szBox, const rect& rc);

	// 计算rcAll中布局box之后剩余的rect区域
	// @align: box相对rect的对齐方式
	// @szBox: box的尺寸大小
	// @rcAll: 布局box和rect的区域
	static rect layout(int align, const point2d& szBox, const rect& rcAll);

private:
	KuAlignment() = delete;
};
