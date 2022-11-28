#pragma once
#include "KvLayoutElement.h"
#include <vector>
#include <memory>


// 受https://github.com/randrew/layout启发，实现的线性布局算法

class KcLayoutVector : public KvLayoutElement
{
	using float_t = typename size_t::value_type;

public:

	using KvLayoutElement::KvLayoutElement;

	void arrange(const rect_t& rc) override;

	point2i extraShares() const override {
		return extraShares_ * shareFactor();
	}

	bool rowMajor() const { return rowMajor_; }
	void setRowMajor(bool b) { rowMajor_ = b; }


	/// element的访问方法

	// 返回第idx个元素的指针
	KvLayoutElement* getAt(unsigned idx) const;

	// 将第idx个元素置为ele，该位置的原有元素将被覆盖
	// 当idx超出范围时，自动增长元素容器空间
	void putAt(unsigned idx, KvLayoutElement* ele);

	// 同putAt，但不自动增长空间，超范围时将出现assert失败
	void setAt(unsigned idx, KvLayoutElement* ele);

	// 在第idx个位置插入ele元素，元素数量加1
	// idx <= size()
	void insertAt(unsigned idx, KvLayoutElement* ele);

	// 删除第idx个元素，元素总量减少1个
	void removeAt(unsigned idx);

	// 同removeAt，但不销毁元素对象，而是返回对象指针
	KvLayoutElement* takeAt(unsigned idx);

	// 返回元素ele的位置序号，-1表示未找到ele
	unsigned find(KvLayoutElement* ele) const;

	void take(KvLayoutElement* ele);

	void remove(KvLayoutElement* ele);

	void append(KvLayoutElement* ele);

	unsigned size() const { return elements_.size(); }

	void resize(unsigned numElements) { elements_.resize(numElements); }

	bool empty() const { return size() == 0; }

protected:

	size_t calcSize_() const override;

	float_t calcSizeStacked_(int dim) const; // 计算各元素dim维度尺寸的累计和
	float_t calcSizeOverlayed_(int dim) const; // 计算各元素dim维度尺寸的最大值

	void arrangeStack_(const rect_t& rc, int dim);
	void arrangeOverlay_(const rect_t& rc, int dim);

protected:
	std::vector<std::unique_ptr<KvLayoutElement>> elements_;

	int spacing_{ 0 }; // 每个布局元素之间的留白，暂未使用
	bool rowMajor_{ true }; // true表示按行排列，false表示按列排列

	// 缓存变量
	mutable point2i extraShares_;
};
