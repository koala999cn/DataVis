#pragma once
#include "KcLayoutGrid.h"


// �������棺ͨ������Ԫ�ص����λ�ù�ϵ�Զ��������Ӳ���

class KgLayoutManager
{
	using rect_t = typename KvLayoutElement::rect_t;

public:

	KvLayoutElement* root() { return root_.get(); }
	const KvLayoutElement* root() const { return root_.get(); }
	void setRoot(KvLayoutElement* ele);

	void take(KvLayoutElement* ele);

	// ����Ԫ��ele����������Ԫ��who�����
	// dist = 0����ʾ���ڷ��ã�1��ʾ��1����ЧԪ�ط��ã�2��ʾ��2����ЧԪ�ط��ã�-1��ʾ��Զ����
	bool placeLeft(KvLayoutElement* who, KvLayoutElement* ele, int dist);
	
	// ͬ�ϣ������Ҳ�
	bool placeRight(KvLayoutElement* who, KvLayoutElement* ele, int dist);

	// ͬ�ϣ������ϲ�
	bool placeTop(KvLayoutElement* who, KvLayoutElement* ele, int dist);

	// ͬ�ϣ������²�
	bool placeBottom(KvLayoutElement* who, KvLayoutElement* ele, int dist);

private:

	// �Ѳ�����ϵ�е�who�滻Ϊele�����֮��who������
	bool substitute_(KvLayoutElement* who, KvLayoutElement* ele);

	bool placeSide_(KvLayoutElement* who, KvLayoutElement* ele, int dist, int side);

private:
	std::unique_ptr<KvLayoutElement> root_;
};

