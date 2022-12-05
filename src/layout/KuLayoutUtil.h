#pragma once
#include "KtPoint.h"
#include "KtAABB.h"

class KuLayoutUtil
{
	using float_t = double;

public:

	using size_t = KtPoint<float_t, 2>;
	using rect_t = KtAABB<float_t, 2>;
	using point_t = typename rect_t::point_t;

	// 返回rect大小为box，anchor按照align方式对齐于rect
	// 此处统一按屏幕坐标，即下为+y，上为-y
	static rect_t anchorAlignedRect(const point_t& anchor, const size_t& box, int align);

private:
	KuLayoutUtil() = delete;
};
