#include "KcLayout2d.h"


KcLayout2d::KcLayout2d()
{
	setRowMajor(false);
}


KcLayout2d::KcLayout2d(KvLayoutElement* p)
	: super_(p)
{
	setRowMajor(false);
}


void KcLayout2d::resize(unsigned numRows)
{
	if (rows() != numRows) {
		auto oldRows = rows();
		super_::resize(numRows);
		for (auto i = oldRows; i < numRows; i++) {
			auto vect = new KcLayout1d;
			super_::setAt(i, vect);
		}
	}
}


void KcLayout2d::removeRowAt(unsigned rowIdx)
{
	assert(rowIdx < rows());
	super_::removeAt(rowIdx);
}


void KcLayout2d::insertRowAt(unsigned rowIdx)
{
	assert(rowIdx <= rows());
	super_::insertAt(rowIdx, new super_);
}


std::pair<unsigned, unsigned> KcLayout2d::find(KvLayoutElement* ele) const
{
	for (unsigned i = 0; i < rows(); i++) {
		auto j = rowAt(i)->find(ele);
		if (j != -1)
			return { i, j };
	}

	return { -1, -1 };
}


void KcLayout2d::take(KvLayoutElement* ele)
{
	auto pos = find(ele);
	if (pos.first != -1)
		rowAt(pos.first)->takeAt(pos.second);
}


void KcLayout2d::remove(KvLayoutElement* ele)
{
	auto pos = find(ele);
	if (pos.first != -1)
		rowAt(pos.first)->removeAt(pos.second);
}

