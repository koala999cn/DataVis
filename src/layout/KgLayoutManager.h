#pragma once
#include "KcLayoutGrid.h"


// 布局引擎：通过布局元素的相对位置关系自动构建复杂布局

class KgLayoutManager
{
	using rect_t = typename KvLayoutElement::rect_t;

public:

	KvLayoutElement* root() { return root_.get(); }
	const KvLayoutElement* root() const { return root_.get(); }
	void setRoot(KvLayoutElement* ele);

	void take(KvLayoutElement* ele);

	// 将新元素ele放置在已有元素who的左侧
	// dist = 0，表示紧邻放置，1表示隔1个有效元素放置，2表示隔2个有效元素放置，-1表示尽远放置
	bool placeLeft(KvLayoutElement* who, KvLayoutElement* ele, int dist);
	
	// 同上，放置右侧
	bool placeRight(KvLayoutElement* who, KvLayoutElement* ele, int dist);

	// 同上，放置上侧
	bool placeTop(KvLayoutElement* who, KvLayoutElement* ele, int dist);

	// 同上，放置下侧
	bool placeBottom(KvLayoutElement* who, KvLayoutElement* ele, int dist);

private:

	// 把布局体系中的who替换为ele，完成之后who被销毁
	bool substitute_(KvLayoutElement* who, KvLayoutElement* ele);

	bool placeSide_(KvLayoutElement* who, KvLayoutElement* ele, int dist, int side);

private:
	std::unique_ptr<KvLayoutElement> root_;
};

