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

	// 返回基于锚点对齐的rect
	// @anchor: 基准锚点
	// @contentSize: rect的尺寸
	// @align: anchor相对于返回rect的对齐方式
	// 此处统一按屏幕坐标，即bottom为+y，top为-y
	static rect_t anchorAlignedRect(const point_t& anchor, const size_t& contentSize, int align);

	// 返回基于边框外对齐的rect
	// @base: 基准边框
	// @contentSize: rect的尺寸. 若contentSize[i] == 0，则rect.extent[i] = base.extent[i]
	// @align: rect相对于base的对齐方式
	// 此处统一按屏幕坐标，即bottom为+y，top为-y
	static rect_t outterAlignedRect(const rect_t& base, const size_t& contentSize, int align);

private:
	KuLayoutUtil() = delete;
};
