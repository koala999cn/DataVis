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

	// ���ػ���ê������rect
	// @anchor: ��׼ê��
	// @contentSize: rect�ĳߴ�
	// @align: anchor����ڷ���rect�Ķ��뷽ʽ
	// �˴�ͳһ����Ļ���꣬��bottomΪ+y��topΪ-y
	static rect_t anchorAlignedRect(const point_t& anchor, const size_t& contentSize, int align);

	// ���ػ��ڱ߿�������rect
	// @base: ��׼�߿�
	// @contentSize: rect�ĳߴ�. ��contentSize[i] == 0����rect.extent[i] = base.extent[i]
	// @align: rect�����base�Ķ��뷽ʽ
	// �˴�ͳһ����Ļ���꣬��bottomΪ+y��topΪ-y
	static rect_t outterAlignedRect(const rect_t& base, const size_t& contentSize, int align);

private:
	KuLayoutUtil() = delete;
};
