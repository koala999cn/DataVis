#pragma once
#include "KcLayoutVector.h"


class KcLayoutFold : public KcLayoutVector
{
public:


private:
	std::vector<std::vector<KvLayoutElement*>> layout_() const;

private:
	int maxItemsPerLine_{ 0 }; // ÿ�л�ÿ������item��Ŀ��������ỻ�л��С�<=0����������
	int rowSpacing_{ 0 }; // �Զ����е��м��
};