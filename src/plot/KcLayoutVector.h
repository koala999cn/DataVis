#pragma once
#include "KvLayoutElement.h"
#include <vector>
#include <memory>


// 线性布局

class KcLayoutVector : public KvLayoutElement
{
public:

	void setOutterRect(const rect_t& rc) override;

private:
	size_t calcContentSize_() const override;

	std::vector<std::vector<KvLayoutElement*>> layout_() const;

private:
	std::vector<std::unique_ptr<KvLayoutElement>> elements_;

	int colSpacing_{ 0 }, rowSpacing_{ 0 };
	bool rowMajor_{ true }; // true表示按行排列，false表示按列排列
	int wraps_{ 0 }; // 每行或每列最大的item数目，超过则会换行或换列。<=0代表无限制
};
