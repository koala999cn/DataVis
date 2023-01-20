#include "KcLayout1d.h"


void KcLayout1d::arrange_(int dim, float_t lower, float_t upper)
{
	if (rowMajor_ ^ dim == 0)
		arrangeOverlay_(dim, lower, upper);
	else 
		arrangeStack_(dim, lower, upper);
}


KcLayout1d::size_t KcLayout1d::calcSize_(void* cxt) const
{
	for (auto& i : elements())
		if (i) i->calcSize(cxt);

	return  rowMajor_ ? size_t(calcSizeStacked_(0), calcSizeOverlayed_(1))
		              : size_t(calcSizeOverlayed_(0), calcSizeStacked_(1));
}


KcLayout1d::float_t KcLayout1d::calcSizeStacked_(int dim) const
{
	float_t sz(0);
	int squeezedItems(0);

	for (auto& i : elements()) {
		if (i) {
			sz += i->expectRoom()[dim];
			squeezedItems += i->extraShares()[dim];
		}
	}
	
	extraShares_[dim] = squeezedItems;

	return sz;
}


KcLayout1d::float_t KcLayout1d::calcSizeOverlayed_(int dim) const
{
	float_t sz(0);
	int shares(0);

	for (auto& i : elements()) {
		if (i) {
			sz = std::max(sz, i->expectRoom()[dim]);
			shares = std::max(shares, i->extraShares()[dim]);
		}
	}

	extraShares_[dim] = shares;

	return sz;
}


void KcLayout1d::arrangeOverlay_(int dim, float_t lower, float_t upper)
{
	__super::arrange_(dim, lower, upper);
	for (auto& i : elements())
		if (i) i->arrange_(dim, lower, upper);
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
