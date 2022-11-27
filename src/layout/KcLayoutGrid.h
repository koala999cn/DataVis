#pragma once
#include "KcLayoutVector.h"


// 网格布局，底层算法依赖KcLayoutVector实现

class KcLayoutGrid : public KcLayoutVector
{
	using super_ = KcLayoutVector;

public:

	KcLayoutGrid();

	void resize(unsigned numRows, unsigned numCols);

	unsigned rows() const { return super_::size(); }
	unsigned cols() const;

	KcLayoutVector* rowAt(unsigned rowIdx) const;

	void removeRowAt(unsigned rowIdx);

	void removeColAt(unsigned colIdx);

	/// element的访问方法

	// 返回[rowIdx, colIdx]位置的元素指针
	// rowIdx < rows(), colIdx < cols()
	KvLayoutElement* getAt(unsigned rowIdx, unsigned colIdx) const;

	// 设定[rowIdx, colIdx]位置元素为ele，该位置的原有元素将被覆盖
	// 当[rowIdx, colIdx]超出范围时，自动增长元素容器空间
	void putAt(unsigned rowIdx, unsigned colIdx, KvLayoutElement* ele);

	// 同putAt，但不自动增长空间，超范围时将出现assert失败
	void setAt(unsigned rowIdx, unsigned colIdx, KvLayoutElement* ele);

	// 在[rowIdx, colIdx]位置插入ele元素，grid的行、列数量均加1
	// rowIdx <= rows(), colIdx <= cols()
	void insertAt(unsigned rowIdx, unsigned colIdx, KvLayoutElement* ele);

	// 删除[rowIdx, colIdx]位置的元素，元素总量减少1个
	// rowIdx < rows(), colIdx < cols()
	void removeAt(unsigned rowIdx, unsigned colIdx);

	// 同removeAt，但不销毁元素对象，而是返回对象指针
	KvLayoutElement* takeAt(unsigned rowIdx, unsigned colIdx);

	// 返回元素ele的位置序号，-1表示未找到ele
	std::pair<unsigned, unsigned> find(KvLayoutElement* ele) const;

	void take(KvLayoutElement* ele);

	void remove(KvLayoutElement* ele);

	// 相当于insertAt(rows(), cols(), ele)
	void append(KvLayoutElement* ele);

};
