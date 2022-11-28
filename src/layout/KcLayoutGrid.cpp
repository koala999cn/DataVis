#include "KcLayoutGrid.h"


KcLayoutGrid::KcLayoutGrid()
{
	setRowMajor(false);
}


KcLayoutGrid::KcLayoutGrid(KvLayoutElement* p)
	: KcLayoutVector(p)
{
	setRowMajor(false);
}


unsigned KcLayoutGrid::cols() const
{
	return empty() ? 0 : rowAt(0)->size();
}


KcLayoutVector* KcLayoutGrid::rowAt(unsigned rowIdx) const
{
	assert(dynamic_cast<KcLayoutVector*>(super_::getAt(rowIdx)));
	return dynamic_cast<KcLayoutVector*>(super_::getAt(rowIdx));
}


void KcLayoutGrid::removeRowAt(unsigned rowIdx)
{
	assert(rowIdx < rows());
	super_::removeAt(rowIdx);
}


void KcLayoutGrid::removeColAt(unsigned colIdx)
{
	assert(colIdx < cols());
	for (unsigned i = 0; i < rows(); i++)
		rowAt(i)->removeAt(colIdx);
}


void KcLayoutGrid::resize(unsigned numRows, unsigned numCols)
{
	if (numCols != cols()) {
		for (unsigned i = 0; i < rows(); i++)
			rowAt(i)->resize(numCols);
	}

	auto oldRows = rows();
	super_::resize(numRows);
	for (auto i = oldRows; i < numRows; i++) {
		auto vect = new KcLayoutVector;
		vect->resize(numCols);
		super_::putAt(i, vect);
	}
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
	assert(rowIdx <= rows() && colIdx <= cols());

	for (unsigned i = 0; i < rows(); i++)
		rowAt(i)->insertAt(colIdx, nullptr);

	auto row = new KcLayoutVector;
	row->resize(cols());
	row->setAt(colIdx, ele);
	super_::insertAt(rowIdx, row);
	if (ele) ele->setParent(this); // 修正parent
}


void KcLayoutGrid::removeAt(unsigned rowIdx, unsigned colIdx)
{
	rowAt(rowIdx)->setAt(colIdx, nullptr);
}


KvLayoutElement* KcLayoutGrid::takeAt(unsigned rowIdx, unsigned colIdx)
{
	auto row = rowAt(rowIdx);
	auto ele = row->takeAt(colIdx);
	row->insertAt(colIdx, nullptr); // 把删掉的元素再填回去nullptr
	return ele;
}


std::pair<unsigned, unsigned> KcLayoutGrid::find(KvLayoutElement* ele) const
{
	for (unsigned i = 0; i < rows(); i++) {
		auto j = rowAt(i)->find(ele);
		if (j != -1)
			return { i, j };
	}

	return { -1, -1 };
}


void KcLayoutGrid::take(KvLayoutElement* ele)
{
	auto pos = find(ele);
	if (pos.first != -1)
		takeAt(pos.first, pos.second);
}


void KcLayoutGrid::remove(KvLayoutElement* ele)
{
	auto pos = find(ele);
	if (pos.first != -1)
		removeAt(pos.first, pos.second);
}


void KcLayoutGrid::append(KvLayoutElement* ele)
{
	insertAt(rows(), cols(), ele);
}
