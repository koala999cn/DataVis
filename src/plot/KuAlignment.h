#pragma once
#include "KtAABB.h"


enum KeAlignment
{
	k_align_center = 0x00,
	k_align_left = 0x01,
	k_align_right = 0x02,
	k_align_top = 0x04,
	k_align_bottom = 0x08,

	// ��λ�ڱ߿��������ʱ��ʹ������2��ö��������ˮƽ���������ȼ�
	// ���磬���k_align_left��k_align_top�������ã���
	//   -- ��k_align_vert_first��Чʱ��λ�ڱ߿�Ķ���λ�ÿ�����룻
	//   -- ��k_align_horz_first��Чʱ��λ�ڱ߿�����λ�ÿ��϶��롣
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

private:
	KuAlignment() = delete;
};
