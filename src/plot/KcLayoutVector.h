#pragma once
#include "KvLayoutElement.h"
#include <vector>
#include <memory>


// ���Բ���

class KcLayoutVector : public KvLayoutElement
{
public:

	void setOutterRect(const rect_t& rc) override;

private:
	size_t calcContentSize_() const override;

	std::vector<std::vector<KvLayoutElement*>> layout_() const;

private:
	std::vector<std::unique_ptr<KvLayoutElement>> elements_;

	int colSpacing_{ 0 }, rowSpacing_{ 0 };
	bool rowMajor_{ true }; // true��ʾ�������У�false��ʾ��������
	int wraps_{ 0 }; // ÿ�л�ÿ������item��Ŀ��������ỻ�л��С�<=0����������
};
