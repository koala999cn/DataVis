#pragma once
#include "kgl/base/KtHolder.h"
#include <string>


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
		k_hcenter = 0x04,
		k_top = 0x08,
		k_bottom = 0x10,
		k_vcenter = 0x20,

		// 当位于边框的外侧对齐时，使用以下2个枚举量区分水平和纵向优先级
		// 例如，如果k_align_left和k_align_top均被设置，则
		//   -- 当k_align_vert_first有效时，位于边框的顶端位置靠左对齐；
		//   -- 当k_align_horz_first有效时，位于边框的左端位置靠上对齐。
		k_vert_first = 0x40,
		k_horz_first = 0x80,

		k_outter = 0x100,

		k_horz_location_mask = k_left | k_right | k_hcenter,
		k_vert_location_mask = k_top | k_bottom | k_vcenter,
		k_location_mask = k_horz_location_mask | k_vert_location_mask,
		k_align_first_mask = k_vert_first | k_horz_first
	};


	int location() const {
		return inside() & k_location_mask;
	}

	void setLocation(int loc) {
		inside() &= ~k_location_mask;
		inside() |= loc;
	}

	int alginFirst() const {
		return inside() & k_align_first_mask;
	}

	void setAlignFirst(int first) {
		inside() &= ~k_align_first_mask;
		inside() |= first;
	}

	bool sameLocation(KeAlignment rhs) const {
		return location() == rhs.location();
	}

	bool sameAlginFirst(KeAlignment rhs) const {
		return alginFirst() == rhs.alginFirst();
	}

	bool outter() const { return inside() & k_outter; }
	bool inner() const { return !outter(); }

	bool hfill() const { return !(inside() & k_horz_location_mask); }
	bool vfill() const { return !(inside() & k_vert_location_mask); }

	void toggleSide() {
		inside() ^= k_outter;
	}

	void toggleAlginFirst() {
		inside() ^= k_align_first_mask;
	}

	// left -> right 或者 right -> left
	void toggleLeftRight() {
		if (inside() & (k_left | k_right))
			inside() ^= (k_left | k_right);
	}

	// top -> bottom 或者 bottom -> top
	void toggleTopBottom() {
		if (inside() & (k_top | k_bottom))
			inside() ^= (k_top | k_bottom);
	}

	std::string format() const {
		std::string str;
		if (inside() & k_left) str += "|left";
		if (inside() & k_right) str += "|right";
		if (inside() & k_hcenter) str += "|hcenter";
		if (inside() & k_top) str += "|top";
		if (inside() & k_bottom) str += "|bottom";
		if (inside() & k_vcenter) str += "|vcenter";

		if (str.empty()) str = "fill";
		else str.erase(str.begin());

		if (inside() & k_vert_first) str += "|vert-first";
		if (inside() & k_horz_first) str += "|horz-first";
		if (inside() & k_outter) str += "|outter";

		return str;
	}
};
