#pragma once
#include "KcLayoutGrid.h"


// �������棺ʵ�ָ��Ӳ��ֵ��Զ�����

class KgLayoutManager
{
public:

	void addElement(KvLayoutElement* ele);

	// ����Ԫ��ele����������Ԫ��who�����
	// dist = 0����ʾ���ڷ��ã�1��ʾ��1����ЧԪ�ط��ã�2��ʾ��2����ЧԪ�ط��ã�-1��ʾ��Զ����
	bool placeLeft(KvLayoutElement* who, KvLayoutElement* ele, int dist);
	
	void placeRight();

	void placeTop();

	void placeBottom();

private:

	// �Ѳ�����ϵ�е�who�滻Ϊele�����֮��who������
	bool substitute_(KvLayoutElement* who, KvLayoutElement* ele);

private:
	std::unique_ptr<KvLayoutElement> root_;
};

