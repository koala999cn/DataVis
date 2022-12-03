#pragma once
#include "KvLayoutContainer.h"
#include <vector>
#include <memory>


// ��https://github.com/randrew/layout������ʵ�ֵ����Բ����㷨

class KcLayout1d : public KvLayoutContainer
{
	using float_t = typename size_t::value_type;
	using super_ = KvLayoutContainer;

public:

	using super_::super_;

	void arrange(const rect_t& rc) override;

	point2i extraShares() const override {
		return extraShares_ * shareFactor();
	}

	bool rowMajor() const { return rowMajor_; }
	void setRowMajor(bool b) { rowMajor_ = b; }


protected:

	size_t calcSize_(void* cxt) const override;

	float_t calcSizeStacked_(int dim) const; // �����Ԫ��dimά�ȳߴ���ۼƺ�
	float_t calcSizeOverlayed_(int dim) const; // �����Ԫ��dimά�ȳߴ�����ֵ

	void arrangeStack_(const rect_t& rc, int dim);
	void arrangeOverlay_(const rect_t& rc, int dim);

protected:

	int spacing_{ 0 }; // ÿ������Ԫ��֮������ף���δʹ��
	bool rowMajor_{ true }; // true��ʾ�������У�false��ʾ��������

	// �������
	mutable point2i extraShares_;
};
