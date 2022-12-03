#include "KcLayout1d.h"


void KcLayout1d::arrange(const rect_t& rc)
{
	if (rowMajor_) {
        if (rc.width()) arrangeStack_(rc, 0);
		if (rc.height()) arrangeOverlay_(rc, 1);
	}
	else {
        if (rc.width()) arrangeOverlay_(rc, 0);
        if (rc.height()) arrangeStack_(rc, 1);
	}
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


void KcLayout1d::arrangeOverlay_(const rect_t& rc, int dim)
{
	__super::arrange_(rc, dim);
	auto rcLay = rc;
	rcLay.upper()[!dim] = rcLay.lower()[!dim]; // 屏蔽另一个维度
	for (auto& i : elements())
		if (i) i->arrange(rcLay);
}


void KcLayout1d::arrangeStack_(const rect_t& rc, int dim)
{
	__super::arrange_(rc, dim);

	auto unusedSpace = iRect_.upper()[dim] - iRect_.lower()[dim];
	auto fixedSpace = expectRoom()[dim];
	auto extraSpace = std::max(0., unusedSpace - fixedSpace);

	// TODO: 1. 暂时使用均匀分配策略; 2. 未考虑extraShares == 0时，仍有extraSpace的情况
	auto spacePerShare = extraShares()[dim] ? extraSpace / extraShares()[dim] : 0;

	rect_t rcItem = iRect_;
	rcItem.upper()[!dim] = rcItem.lower()[!dim]; // 屏蔽另一个维度
	for (auto& i : elements()) {
		if (i == nullptr)
			continue;

		// 支持fixd-item和squeezed-item的混合体
		auto itemSpace = i->expectRoom()[dim] + i->extraShares()[dim] * spacePerShare;

		rcItem.upper()[dim] = rcItem.lower()[dim] + itemSpace;
		i->arrange(rcItem);

		rcItem.lower() = rcItem.upper();
	}
}
