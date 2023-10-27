#pragma once

class KvLayoutElement;
class KcLayoutOverlay;

// 布局帮助类：通过相对位置关系管理布局元素

class KuLayoutHelper
{
public:

	static void take(KvLayoutElement* ele);

	// 将新元素ele放置在已有元素who的左侧
	// dist = 0，表示紧邻放置，1表示隔1个有效元素放置，2表示隔2个有效元素放置，-1表示尽远放置
	// assert(who->parent() != 0)
	static void placeLeft(KvLayoutElement* who, KvLayoutElement* ele, int dist);
	
	// 同上，放置右侧
	static void placeRight(KvLayoutElement* who, KvLayoutElement* ele, int dist);

	// 同上，放置上侧
	static void placeTop(KvLayoutElement* who, KvLayoutElement* ele, int dist);

	// 同上，放置下侧
	static void placeBottom(KvLayoutElement* who, KvLayoutElement* ele, int dist);

	static void align(KcLayoutOverlay* target, KvLayoutElement* ele);

private:

	KuLayoutHelper() = delete;
};

