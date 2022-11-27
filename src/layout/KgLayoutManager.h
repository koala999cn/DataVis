#pragma once
#include "KcLayoutGrid.h"


// 布局引擎：实现复杂布局的自动管理

class KgLayoutManager
{
public:

	void addElement(KvLayoutElement* ele);

	// 将新元素ele放置在已有元素who的左侧
	// dist = 0，表示紧邻放置，1表示隔1个有效元素放置，2表示隔2个有效元素放置，-1表示尽远放置
	bool placeLeft(KvLayoutElement* who, KvLayoutElement* ele, int dist);
	
	void placeRight();

	void placeTop();

	void placeBottom();

private:

	// 把布局体系中的who替换为ele，完成之后who被销毁
	bool substitute_(KvLayoutElement* who, KvLayoutElement* ele);

private:
	std::unique_ptr<KvLayoutElement> root_;
};

