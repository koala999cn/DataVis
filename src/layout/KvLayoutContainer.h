#pragma once
#include "KvLayoutElement.h"
#include <memory>
#include <assert.h>


// ������Ԫ�صĲ�����������Ҫ�ṩ��Ԫ�صĶ�д����

class KvLayoutContainer : public KvLayoutElement
{
public:
	using container_type = std::vector<std::unique_ptr<KvLayoutElement>>;

	using KvLayoutElement::KvLayoutElement;

	unsigned size() const { return elements_.size(); }

	void resize(unsigned numElements) { elements_.resize(numElements); }

	bool empty() const { return size() == 0; }

	KvLayoutElement* getAt(unsigned idx) const {
		assert(idx < size());
		return std::next(elements_.cbegin(), idx)->get();
	}

	// ͬputAt�������Զ������ռ䣬����Χʱ������assertʧ��
	void setAt(unsigned idx, KvLayoutElement* ele) {
		assert(idx < size());
		assert(ele == nullptr || ele->parent() == nullptr);
		if (ele) ele->setParent(this);
		std::next(elements_.begin(), idx)->reset(ele);
	}

	// �ڵ�idx��λ�ò���eleԪ�أ�Ԫ��������1
	// idx <= size()
	void insertAt(unsigned idx, KvLayoutElement* ele) {
		assert(idx <= size());
		assert(ele == nullptr || ele->parent() == nullptr);
		if (ele) ele->setParent(this);
		elements_.emplace(std::next(elements_.cbegin(), idx), ele);
	}

	// ��ӵ���һ����λ��(Ԫ��Ϊnullptr)
	void insert(KvLayoutElement* ele) {
		auto nullPos = find(nullptr);
		if (nullPos == -1)
			append(ele);
		else
			setAt(nullPos, ele);
	}

	// ��ӵ�ĩβ
	void append(KvLayoutElement* ele) {
		assert(ele == nullptr || ele->parent() == nullptr);
		if (ele) ele->setParent(this);
		elements_.emplace_back(ele);
	}

	// ����idx��Ԫ����Ϊele����λ�õ�ԭ��Ԫ�ؽ�������
	// ��idx������Χʱ���Զ�����Ԫ�������ռ�
	void putAt(unsigned idx, KvLayoutElement* ele) {
		if (idx >= size())
			resize(idx + 1);
		setAt(idx, ele);
	}

	// ɾ����idx��Ԫ�أ�Ԫ����������1��
	void removeAt(unsigned idx) {
		assert(idx < size());
		elements_.erase(std::next(elements_.cbegin(), idx));
	}

	// ���ص�idx��λ�õ�Ԫ�ض���ָ�룬ͬʱ�ͷŶԸ�Ԫ�ص��������ڹ���Ȩ�ޣ�
	// NB: �÷��������������ߴ磬��������size����
	KvLayoutElement* takeAt(unsigned idx) {
		assert(idx < size());

		auto pos = std::next(elements_.begin(), idx);
		if (pos->get())
			pos->get()->setParent(nullptr);

		return pos->release();
	}

	// ����Ԫ��ele��λ����ţ�-1��ʾδ�ҵ�ele
	unsigned find(KvLayoutElement* ele) const {
		for (auto iter = std::cbegin(elements_); iter != std::cend(elements_); iter++)
			if (iter->get() == ele)
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

	// this�Ƿ�Ϊele������
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
