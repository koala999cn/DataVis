#pragma once
#include "KcLayout2d.h"


// 网格布局，每行的列数相同，同列元素的布局宽度相同

class KcLayoutGrid : public KcLayout2d
{
	using float_t = typename size_t::value_type;
	using super_ = KcLayout2d;

public:

	using super_::super_;

	unsigned cols() const;

	void resize(unsigned numRows, unsigned numCols);

	void removeColAt(unsigned colIdx);

	void insertRowAt(unsigned rowIdx);

	void insertColAt(unsigned colIdx);

	/// 为保持存储的严整（各行的列数相同），重新实现element的访问方法

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

	void remove(KvLayoutElement* ele);


	void arrange(const rect_t& rc) final;

protected:
	size_t calcSize_(void* cxt) const override;

private:

	// 模拟实现0维度的arrangeStack_
	void arrangeColStack_(const rect_t& rc);

private:
	// calcSize的缓存变量. 用于保存每列的尺寸数据，first值为列的固定尺寸，second值为列的extra份额
	mutable std::vector<std::pair<float_t, int>> szCols_;
};
