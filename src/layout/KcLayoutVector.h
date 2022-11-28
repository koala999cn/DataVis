#pragma once
#include "KvLayoutElement.h"
#include <vector>
#include <memory>


// ��https://github.com/randrew/layout������ʵ�ֵ����Բ����㷨

class KcLayoutVector : public KvLayoutElement
{
	using float_t = typename size_t::value_type;

public:

	using KvLayoutElement::KvLayoutElement;

	void arrange(const rect_t& rc) override;

	point2i extraShares() const override {
		return extraShares_ * shareFactor();
	}

	bool rowMajor() const { return rowMajor_; }
	void setRowMajor(bool b) { rowMajor_ = b; }


	/// element�ķ��ʷ���

	// ���ص�idx��Ԫ�ص�ָ��
	KvLayoutElement* getAt(unsigned idx) const;

	// ����idx��Ԫ����Ϊele����λ�õ�ԭ��Ԫ�ؽ�������
	// ��idx������Χʱ���Զ�����Ԫ�������ռ�
	void putAt(unsigned idx, KvLayoutElement* ele);

	// ͬputAt�������Զ������ռ䣬����Χʱ������assertʧ��
	void setAt(unsigned idx, KvLayoutElement* ele);

	// �ڵ�idx��λ�ò���eleԪ�أ�Ԫ��������1
	// idx <= size()
	void insertAt(unsigned idx, KvLayoutElement* ele);

	// ɾ����idx��Ԫ�أ�Ԫ����������1��
	void removeAt(unsigned idx);

	// ͬremoveAt����������Ԫ�ض��󣬶��Ƿ��ض���ָ��
	KvLayoutElement* takeAt(unsigned idx);

	// ����Ԫ��ele��λ����ţ�-1��ʾδ�ҵ�ele
	unsigned find(KvLayoutElement* ele) const;

	void take(KvLayoutElement* ele);

	void remove(KvLayoutElement* ele);

	void append(KvLayoutElement* ele);

	unsigned size() const { return elements_.size(); }

	void resize(unsigned numElements) { elements_.resize(numElements); }

	bool empty() const { return size() == 0; }

protected:

	size_t calcSize_() const override;

	float_t calcSizeStacked_(int dim) const; // �����Ԫ��dimά�ȳߴ���ۼƺ�
	float_t calcSizeOverlayed_(int dim) const; // �����Ԫ��dimά�ȳߴ�����ֵ

	void arrangeStack_(const rect_t& rc, int dim);
	void arrangeOverlay_(const rect_t& rc, int dim);

protected:
	std::vector<std::unique_ptr<KvLayoutElement>> elements_;

	int spacing_{ 0 }; // ÿ������Ԫ��֮������ף���δʹ��
	bool rowMajor_{ true }; // true��ʾ�������У�false��ʾ��������

	// �������
	mutable point2i extraShares_;
};
