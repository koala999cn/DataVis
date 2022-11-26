#pragma once
#include "KvLayoutElement.h"
#include <vector>
#include <memory>


// ��https://github.com/randrew/layout������ʵ�ֵ����Բ����㷨

class KcLayoutVector : public KvLayoutElement
{
	using float_t = typename size_t::value_type;

public:

	void arrange(const rect_t& rc) override;

	point2i squeezeNeeded() const override {
		return squeezeNeeded_;
	}

protected:

	size_t calcSize_() const override;

	float_t calcSizeStacked_(int dim) const;
	float_t calcSizeOverlayed_(int dim) const;

	void arrangeStack_(const rect_t& rc, int dim);
	void arrangeOverlay_(const rect_t& rc, int dim);

protected:
	std::vector<std::unique_ptr<KvLayoutElement>> elements_;

	int spacing_{ 0 }; // ÿ������Ԫ��֮������ף���δʹ��
	bool rowMajor_{ true }; // true��ʾ�������У�false��ʾ��������

	// �������
	mutable point2i squeezeNeeded_;
};
