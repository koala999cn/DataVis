#include "KcLayoutGrid.h"


unsigned KcLayoutGrid::cols() const
{
	return empty() ? 0 : rowAt(0)->size();
}


void KcLayoutGrid::resize(unsigned numRows, unsigned numCols)
{
	super_::resize(numRows);
	assert(rows() == numRows);

	for (unsigned i = 0; i < numRows; i++)
		rowAt(i)->resize(numCols);
	assert(cols() == numCols);
}


void KcLayoutGrid::removeColAt(unsigned colIdx)
{
	assert(colIdx < cols());
	for (unsigned i = 0; i < rows(); i++)
		rowAt(i)->removeAt(colIdx);
}


void KcLayoutGrid::insertRowAt(unsigned rowIdx)
{
	assert(rowIdx <= rows());
	auto numCols = cols();
	super_::insertRowAt(rowIdx);
	rowAt(rowIdx)->resize(numCols);
}


void KcLayoutGrid::insertColAt(unsigned colIdx)
{
	assert(colIdx <= cols());

	for (unsigned i = 0; i < rows(); i++)
		rowAt(i)->insertAt(colIdx, nullptr);
}


KvLayoutElement* KcLayoutGrid::getAt(unsigned rowIdx, unsigned colIdx) const
{
	assert(rowIdx < rows() && colIdx < cols());
	return rowAt(rowIdx)->getAt(colIdx);
}


void KcLayoutGrid::putAt(unsigned rowIdx, unsigned colIdx, KvLayoutElement* ele)
{
	if (rowIdx >= rows() || colIdx >= cols())
		resize(std::max(rowIdx + 1, rows()), std::max(colIdx + 1, cols()));

	setAt(rowIdx, colIdx, ele);
}


void KcLayoutGrid::setAt(unsigned rowIdx, unsigned colIdx, KvLayoutElement* ele)
{
	assert(rowIdx < rows() && colIdx < cols());
	rowAt(rowIdx)->setAt(colIdx, ele);
	if (ele) ele->setParent(this); // 修正parent
}


void KcLayoutGrid::insertAt(unsigned rowIdx, unsigned colIdx, KvLayoutElement* ele)
{
	insertColAt(colIdx);
	insertRowAt(rowIdx);
	setAt(rowIdx, colIdx, ele);
}


void KcLayoutGrid::removeAt(unsigned rowIdx, unsigned colIdx)
{
	rowAt(rowIdx)->setAt(colIdx, nullptr);
}


KvLayoutElement* KcLayoutGrid::takeAt(unsigned rowIdx, unsigned colIdx)
{
	assert(rowIdx < rows() && colIdx < cols());
	return rowAt(rowIdx)->takeAt(colIdx);
}


void KcLayoutGrid::remove(KvLayoutElement* ele)
{
	auto pos = find(ele);
	if (pos.first != -1)
		rowAt(pos.first)->setAt(pos.second, nullptr);
}


void KcLayoutGrid::arrange(const rect_t& rc)
{
	assert(!rowMajor_);

	if (rc.height()) arrangeStack_(rc, 1);
	if (rc.width()) arrangeColStack_(rc);
}


KcLayoutGrid::size_t KcLayoutGrid::calcSize_(void* cxt) const
{
	auto sz = super_::calcSize_(cxt);

	// 统计每列的尺寸信息到szCols_
	szCols_.resize(cols());
	float_t contentWidth(0);
	int widthShares(0);
	for (unsigned c = 0; c < cols(); c++) {
		szCols_[c] = { 0, 0 };
		for (unsigned r = 0; r < rows(); r++) {
			auto ele = rowAt(r)->getAt(c);
			if (ele) {
				szCols_[c].first = std::max(szCols_[c].first, ele->expectRoom()[0]);
				szCols_[c].second = std::max(szCols_[c].second, ele->extraShares()[0]);
			}
		}

		contentWidth += szCols_[c].first;
		widthShares += szCols_[c].second;
	}

	sz[0] = contentWidth;
	extraShares_[0] = widthShares;

	return sz;
}


void KcLayoutGrid::arrangeColStack_(const rect_t& rc)
{
	align_ = 0;
	super_::arrange_(rc, 0); // 初始化iRect_

	auto unusedSpace = iRect_.width();
	auto fixedSpace = contentSize()[0];
	auto extraSpace = std::max(0., unusedSpace - fixedSpace);

	auto spacePerShare = extraShares()[0] ? extraSpace / extraShares()[0] : 0;

	rect_t rcItem = iRect_;
	rcItem.setExtent(1, 0); // 屏蔽另一个维度
	for (unsigned c = 0; c < cols(); c++) {
		// 支持fixd-item和squeezed-item的混合体
		rcItem.setExtent(0, szCols_[c].first + szCols_[c].second * spacePerShare);

		for (unsigned r = 0; r < rows(); r++) {
			// TODO: 此处没有调用rowAt(r)的arrange，所以rowAt(r)的x维度尺寸为0
			// rowAt(r)->arrange_(rcItem, 0); 

			auto ele = rowAt(r)->getAt(c); 
			if (ele) ele->arrange(rcItem);
		}

		rcItem.lower() = rcItem.upper();
	}
}
