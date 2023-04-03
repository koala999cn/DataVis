#pragma once
#include "KvLayoutElement.h"
#include <memory>
#include <assert.h>


// 包含子元素的布局容器，主要提供子元素的读写访问

class KvLayoutContainer : public KvLayoutElement
{
public:
	using container_type = std::vector<KvLayoutElement*>; // 不使用unique智能指针，否则KvLayoutElement无法实现拷贝构造

	using KvLayoutElement::KvLayoutElement;

	unsigned size() const { return elements_.size(); }

	void resize(unsigned numElements) { elements_.resize(numElements); }

	bool empty() const { return size() == 0; }

	KvLayoutElement* getAt(unsigned idx) const {
		assert(idx < size());
		return *std::next(elements_.cbegin(), idx);
	}

	// 同putAt，但不自动增长空间，超范围时将出现assert失败
	// idx位置的原对象将被销毁
	void setAt(unsigned idx, KvLayoutElement* ele) {
		assert(idx < size());
		assert(ele == nullptr || ele->parent() == nullptr);
		if (ele) ele->setParent(this);
		auto pos = std::next(elements_.begin(), idx);
		if (*pos) delete *pos;
		*pos = ele;
	}

	// 在第idx个位置插入ele元素，元素数量加1
	// idx <= size()
	void insertAt(unsigned idx, KvLayoutElement* ele) {
		assert(idx <= size());
		assert(ele == nullptr || ele->parent() == nullptr);
		if (ele) ele->setParent(this);
		elements_.emplace(std::next(elements_.cbegin(), idx), ele);
	}

	// 添加到第一个空位置(元素为nullptr)
	void insert(KvLayoutElement* ele) {
		auto nullPos = find(nullptr);
		if (nullPos == -1)
			append(ele);
		else
			setAt(nullPos, ele);
	}

	// 添加到末尾
	void append(KvLayoutElement* ele) {
		assert(ele == nullptr || ele->parent() == nullptr);
		if (ele) ele->setParent(this);
		elements_.emplace_back(ele);
	}

	// 将第idx个元素置为ele，该位置的原有元素将被覆盖
	// 当idx超出范围时，自动增长元素容器空间
	void putAt(unsigned idx, KvLayoutElement* ele) {
		if (idx >= size())
			resize(idx + 1);
		setAt(idx, ele);
	}

	// 删除第idx个元素，元素总量减少1个
	void removeAt(unsigned idx) {
		assert(idx < size());
		auto pos = std::next(elements_.cbegin(), idx);
		if (*pos) delete* pos;
		elements_.erase(pos);
	}

	// 返回第idx个位置的元素对象指针，同时释放对该元素的生命周期管理权限，
	// NB: 该方法不缩减容器尺寸，保持容器size不变
	KvLayoutElement* takeAt(unsigned idx) {
		assert(idx < size());

		auto pos = std::next(elements_.begin(), idx);
		auto ele = *pos;
	
		if (ele) {
			ele->setParent(nullptr);
			*pos = nullptr;
		}

		return ele;
	}

	// 返回元素ele的位置序号，-1表示未找到ele
	unsigned find(KvLayoutElement* ele) const {
		for (auto iter = std::cbegin(elements_); iter != std::cend(elements_); iter++)
			if (*iter == ele)
				return std::distance(std::cbegin(elements_), iter);

		return -1;
	}

	void take(KvLayoutElement* ele) {
		auto pos = find(ele);
		if (pos != -1)
			takeAt(pos);
	}

	void remove(KvLayoutElement* ele) {
		auto pos = find(ele);
		if (pos != -1)
			removeAt(pos);
	}

	// this是否为ele的祖先
	bool isAncestorOf(KvLayoutElement* ele) const {
		assert(ele);
		auto p = ele->parent();
		while (p) {
			if (p == this)
				return true;
			p = p->parent();
		}

		return false;
	}

	const container_type& elements() const {
		return elements_;
	}


private:
	container_type elements_;
};
