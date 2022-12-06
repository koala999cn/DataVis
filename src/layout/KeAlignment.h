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

		// ��λ�ڱ߿��������ʱ��ʹ������2��ö��������ˮƽ���������ȼ�
		// ���磬���k_align_left��k_align_top�������ã���
		//   -- ��k_align_vert_first��Чʱ��λ�ڱ߿�Ķ���λ�ÿ�����룻
		//   -- ��k_align_horz_first��Чʱ��λ�ڱ߿�����λ�ÿ��϶��롣
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

	void toggleSide() {
		inside() ^= k_outter;
	}

	void toggleAlginFirst() {
		inside() ^= k_align_first_mask;
	}

	// left -> right ���� right -> left
	void toggleLeftRight() {
		if (inside() & (k_left | k_right))
			inside() ^= (k_left | k_right);
	}

	// top -> bottom ���� bottom -> top
	void toggleTopBottom() {
		if (inside() & (k_top | k_bottom))
			inside() ^= (k_top | k_bottom);
	}
};
