#pragma once
#include "KvLayoutContainer.h"
#include <vector>
#include <memory>


// ��https://github.com/randrew/layout������ʵ�ֵ����Բ����㷨

class KcLayout1d : public KvLayoutContainer
{
	using super_ = KvLayoutContainer;

public:

	using super_::super_;

	void arrange_(int dim, float_t lower, float_t upper) override;

	point2i extraShares() const override {
		return extraShares_ * shareFactor();
	}

	bool rowMajor() const { return rowMajor_; }
	void setRowMajor(bool b) { rowMajor_ = b; }


protected:

	size_t calcSize_(void* cxt) const override;

	std::pair<float_t, int> calcSizeStacked_(int dim) const; // �����Ԫ��dimά�ȳߴ���ۼƺ�
	std::pair<float_t, int> calcSizeOverlayed_(int dim) const; // �����Ԫ��dimά�ȳߴ�����ֵ

	void arrangeStack_(int dim, float_t lower, float_t upper);
	void arrangeOverlay_(int dim, float_t lower, float_t upper);

protected:

	int spacing_{ 0 }; // ÿ������Ԫ��֮������ף���δʹ��
	bool rowMajor_{ true }; // true��ʾ�������У�false��ʾ��������

	// �������
	mutable point2i extraShares_;
};
