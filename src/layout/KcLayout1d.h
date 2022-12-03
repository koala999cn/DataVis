#pragma once
#include "KvLayoutContainer.h"
#include <vector>
#include <memory>


// 受https://github.com/randrew/layout启发，实现的线性布局算法

class KcLayout1d : public KvLayoutContainer
{
	using float_t = typename size_t::value_type;
	using super_ = KvLayoutContainer;

public:

	using super_::super_;

	void arrange(const rect_t& rc) override;

	point2i extraShares() const override {
		return extraShares_ * shareFactor();
	}

	bool rowMajor() const { return rowMajor_; }
	void setRowMajor(bool b) { rowMajor_ = b; }


protected:

	size_t calcSize_(void* cxt) const override;

	float_t calcSizeStacked_(int dim) const; // 计算各元素dim维度尺寸的累计和
	float_t calcSizeOverlayed_(int dim) const; // 计算各元素dim维度尺寸的最大值

	void arrangeStack_(const rect_t& rc, int dim);
	void arrangeOverlay_(const rect_t& rc, int dim);

protected:

	int spacing_{ 0 }; // 每个布局元素之间的留白，暂未使用
	bool rowMajor_{ true }; // true表示按行排列，false表示按列排列

	// 缓存变量
	mutable point2i extraShares_;
};
