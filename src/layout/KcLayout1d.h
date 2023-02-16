#pragma once
#include "KvLayoutContainer.h"
#include <vector>
#include <memory>


// 受https://github.com/randrew/layout启发，实现的线性布局算法

class KcLayout1d : public KvLayoutContainer
{
	using super_ = KvLayoutContainer;

public:

	using super_::super_;

	void arrange_(int dim, float_t lower, float_t upper) override;

	point2i extraShares() const override {
		return extraShares_ * shareFactor();
	}

	bool rowMajor() const { return rowMajor_; }
	void setRowMajor(bool b) { rowMajor_ = b; }


protected:

	size_t calcSize_(void* cxt) const override;

	std::pair<float_t, int> calcSizeStacked_(int dim) const; // 计算各元素dim维度尺寸的累计和
	std::pair<float_t, int> calcSizeOverlayed_(int dim) const; // 计算各元素dim维度尺寸的最大值

	void arrangeStack_(int dim, float_t lower, float_t upper);
	void arrangeOverlay_(int dim, float_t lower, float_t upper);

protected:

	int spacing_{ 0 }; // 每个布局元素之间的留白，暂未使用
	bool rowMajor_{ true }; // true表示按行排列，false表示按列排列

	// 缓存变量
	mutable point2i extraShares_;
};
