#pragma once
#include "KtHolder.h"


class KeAlignment : public KtHolder<int>
{
	using super_ = KtHolder<int>;

public:

	using super_::super_;

	enum
	{
		k_fill = 0x00,
		k_left = 0x01,
		k_right = 0x02,
		k_vcenter = 0x04,
		k_top = 0x08,
		k_bottom = 0x10,
		k_hcenter = 0x20,

		// 当位于边框的外侧对齐时，使用以下2个枚举量区分水平和纵向优先级
		// 例如，如果k_align_left和k_align_top均被设置，则
		//   -- 当k_align_vert_first有效时，位于边框的顶端位置靠左对齐；
		//   -- 当k_align_horz_first有效时，位于边框的左端位置靠上对齐。
		k_vert_first = 0x40,
		k_horz_first = 0x80,

		k_horz_location_mask = k_left | k_right | k_hcenter,
		k_vert_location_mask = k_top | k_bottom | k_vcenter,
		k_location_mask = k_horz_location_mask | k_vert_location_mask,
		k_side_mask = k_vert_first | k_horz_first
	};


	int location() const {
		return inside() & k_location_mask;
	}

	int side() const {
		return inside() & k_side_mask;
	}

	bool sameLocation(KeAlignment rhs) const {
		return location() == rhs.location();
	}

	bool sameSide(KeAlignment rhs) const {
		return side() == rhs.side();
	}

	bool outter() const { return side(); }

	bool inner() const { return !side(); }

	void toggleSide() {
		if (side())
			inside() &= ~k_side_mask;
		else
			inside() |= k_horz_first; // 默认水平优先
	}

	void toggleHorzFirst() {
		if (side()) inside() ^= k_side_mask;
	}

	// left -> right 或者 right -> left
	void toggleHAlign() {
		if (inside() & (k_left | k_right))
			inside() ^= (k_left | k_right);
	}

	// top -> bottom 或者 bottom -> top
	void toggleVAlign() {
		if (inside() & (k_top | k_bottom))
			inside() ^= (k_top | k_bottom);
	}
};

#if 0
class KuAlignment
{
public:

	using rect = KtAABB<double, 2>;



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


};
#endif