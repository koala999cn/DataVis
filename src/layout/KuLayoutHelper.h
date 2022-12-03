#pragma once

class KvLayoutElement;
class KcLayoutOverlay;

// ���ְ����ࣺͨ�����λ�ù�ϵ������Ԫ��

class KuLayoutHelper
{
public:

	static void take(KvLayoutElement* ele);

	// ����Ԫ��ele����������Ԫ��who�����
	// dist = 0����ʾ���ڷ��ã�1��ʾ��1����ЧԪ�ط��ã�2��ʾ��2����ЧԪ�ط��ã�-1��ʾ��Զ����
	// assert(who->parent() != 0)
	static bool placeLeft(KvLayoutElement* who, KvLayoutElement* ele, int dist);
	
	// ͬ�ϣ������Ҳ�
	static bool placeRight(KvLayoutElement* who, KvLayoutElement* ele, int dist);

	// ͬ�ϣ������ϲ�
	static bool placeTop(KvLayoutElement* who, KvLayoutElement* ele, int dist);

	// ͬ�ϣ������²�
	static bool placeBottom(KvLayoutElement* who, KvLayoutElement* ele, int dist);


private:

	KuLayoutHelper() = delete;
};

