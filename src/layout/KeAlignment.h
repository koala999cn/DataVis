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
			inside() |= k_horz_first; // Ĭ��ˮƽ����
	}

	void toggleHorzFirst() {
		if (side()) inside() ^= k_side_mask;
	}

	// left -> right ���� right -> left
	void toggleHAlign() {
		if (inside() & (k_left | k_right))
			inside() ^= (k_left | k_right);
	}

	// top -> bottom ���� bottom -> top
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



	// ����box��lower��λ��(��Ļ����)
	// @align: box�Ķ��뷽ʽ
	// @szBox: box�ĳߴ��С
	// @rc: box������rect
	static point2d position(int align, const point2d& szBox, const rect& rc);

	// ����rcAll�в���box֮��ʣ���rect����
	// @align: box���rect�Ķ��뷽ʽ
	// @szBox: box�ĳߴ��С
	// @rcAll: ����box��rect������
	static rect layout(int align, const point2d& szBox, const rect& rcAll);


};
#endif