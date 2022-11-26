#pragma once
#include "KvLayoutElement.h"
#include <vector>
#include <memory>


// 受https://github.com/randrew/layout启发，实现的线性布局算法

class KcLayoutVector : public KvLayoutElement
{
	using float_t = typename size_t::value_type;

public:

	void arrange(const rect_t& rc) override;

	point2i squeezeNeeded() const override {
		return squeezeNeeded_;
	}

protected:

	size_t calcSize_() const override;

	float_t calcSizeStacked_(int dim) const;
	float_t calcSizeOverlayed_(int dim) const;

	void arrangeStack_(const rect_t& rc, int dim);
	void arrangeOverlay_(const rect_t& rc, int dim);

protected:
	std::vector<std::unique_ptr<KvLayoutElement>> elements_;

	int spacing_{ 0 }; // 每个布局元素之间的留白，暂未使用
	bool rowMajor_{ true }; // true表示按行排列，false表示按列排列

	// 缓存变量
	mutable point2i squeezeNeeded_;
};
