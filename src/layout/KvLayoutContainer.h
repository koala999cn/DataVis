#pragma once
#include "KvLayoutElement.h"
#include <memory>
#include <assert.h>


// ������Ԫ�صĲ�����������Ҫ�ṩ��Ԫ�صĶ�д����

class KvLayoutContainer : public KvLayoutElement
{
public:
	using container_type = std::vector<KvLayoutElement*>; // ��ʹ��unique����ָ�룬����KvLayoutElement�޷�ʵ�ֿ�������

	using KvLayoutElement::KvLayoutElement;

	unsigned size() const { return elements_.size(); }

	void resize(unsigned numElements) { elements_.resize(numElements); }

	bool empty() const { return size() == 0; }

	KvLayoutElement* getAt(unsigned idx) const {
		assert(idx < size());
		return *std::next(elements_.cbegin(), idx);
	}

	// ͬputAt�������Զ������ռ䣬����Χʱ������assertʧ��
	// idxλ�õ�ԭ���󽫱�����
	void setAt(unsigned idx, KvLayoutElement* ele) {
		assert(idx < size());
		assert(ele == nullptr || ele->parent() == nullptr);
		if (ele) ele->setParent(this);
		auto pos = std::next(elements_.begin(), idx);
		if (*pos) delete *pos;
		*pos = ele;
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
		auto pos = std::next(elements_.cbegin(), idx);
		if (*pos) delete* pos;
		elements_.erase(pos);
	}

	// ���ص�idx��λ�õ�Ԫ�ض���ָ�룬ͬʱ�ͷŶԸ�Ԫ�ص��������ڹ���Ȩ�ޣ�
	// NB: �÷��������������ߴ磬��������size����
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

	// ����Ԫ��ele��λ����ţ�-1��ʾδ�ҵ�ele
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
