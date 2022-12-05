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

	// ����rect��СΪbox��anchor����align��ʽ������rect
	// �˴�ͳһ����Ļ���꣬����Ϊ+y����Ϊ-y
	static rect_t anchorAlignedRect(const point_t& anchor, const size_t& box, int align);

private:
	KuLayoutUtil() = delete;
};
