#pragma once
#include "KcLayout1d.h"


// TODO: 待实现
class KcLayoutFold : public KcLayout1d
{
public:


private:
	std::vector<std::vector<KvLayoutElement*>> layout_() const;

private:
	int maxItemsPerLine_{ 0 }; // 每行或每列最大的item数目，超过则会换行或换列。<=0代表无限制
	int rowSpacing_{ 0 }; // 自动换行的行间距
};
