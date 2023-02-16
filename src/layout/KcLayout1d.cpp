#include "KcLayout1d.h"


void KcLayout1d::arrange_(int dim, float_t lower, float_t upper)
{
	if (rowMajor_ ^ dim == 0)
		arrangeOverlay_(dim, lower, upper );
	else 
		arrangeStack_(dim, lower, upper);
}


KcLayout1d::size_t KcLayout1d::calcSize_(void* cxt) const
{
	for (auto& i : elements())
		if (i) i->calcSize(cxt);

	std::pair<float_t, int> sz[2];
	if (rowMajor_) 
		sz[0] = calcSizeStacked_(0), sz[1] = calcSizeOverlayed_(1);
	else 
		sz[1] = calcSizeStacked_(1), sz[0] = calcSizeOverlayed_(0);

	extraShares_[0] = sz[0].second, extraShares_[1] = sz[1].second;

	return { sz[0].first, sz[1].first };
}


std::pair<KcLayout1d::float_t, int> KcLayout1d::calcSizeStacked_(int dim) const
{
	float_t fixed(0);
	int shares(0);

	for (auto& i : elements()) {
		if (i) {
			fixed += i->expectRoom()[dim];
			shares += i->extraShares()[dim];
		}
	}

	return { fixed, shares };
}


std::pair<KcLayout1d::float_t, int> KcLayout1d::calcSizeOverlayed_(int dim) const
{
	float_t fixed(0);
	int shares(0);

	for (auto& i : elements()) {
		if (i) {
			fixed = std::max(fixed, i->expectRoom()[dim]);
			shares = std::max(shares, i->extraShares()[dim]);
		}
	}

	return { fixed, shares };
}


void KcLayout1d::arrangeOverlay_(int dim, float_t lower, float_t upper)
{
	__super::arrange_(dim, lower, upper);
	for (auto& i : elements())
		if (i) i->arrange_(dim, iRect_.lower()[dim], iRect_.upper()[dim]);
}


void KcLayout1d::arrangeStack_(int dim, float_t lower, float_t upper)
{
	__super::arrange_(dim, lower, upper);

	auto unusedSpace = iRect_.upper()[dim] - iRect_.lower()[dim];
	auto fixedSpace = contentSize()[dim]; // �˴���������expectRoom����������Ҫ����һ�Σ���Ϊ�����rc�ѿ۳�������
	auto extraSpace = unusedSpace - fixedSpace;
	
	// NB: ����fixedSpace > unusedSpace�����
	// �˴��ṩһ����������scale���Ը�Ԫ�ص�fixedSpace���еȱ�����
	auto scale = 1.0;
	if (extraSpace < 0) {
		assert(fixedSpace != 0);
		extraSpace = 0;
		scale = unusedSpace / fixedSpace;
	}

	// TODO: 1. ��ʱʹ�þ��ȷ������; 2. δ����extraShares == 0ʱ������extraSpace�����
	auto spacePerShare = extraShares()[dim] ? extraSpace / extraShares()[dim] : 0;

	auto l = iRect_.lower()[dim];
	for (auto& i : elements()) {
		if (i == nullptr)
			continue;

		// ֧��fixd-item��squeezed-item�Ļ����
		auto expectRoom = i->expectRoom()[dim];
		auto itemSpace = expectRoom * scale + i->extraShares()[dim] * spacePerShare;

		auto u = l + itemSpace;
		i->arrange_(dim, l, u);

		l = u;
	}
}
