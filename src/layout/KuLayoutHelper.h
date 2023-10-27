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
	static void placeLeft(KvLayoutElement* who, KvLayoutElement* ele, int dist);
	
	// ͬ�ϣ������Ҳ�
	static void placeRight(KvLayoutElement* who, KvLayoutElement* ele, int dist);

	// ͬ�ϣ������ϲ�
	static void placeTop(KvLayoutElement* who, KvLayoutElement* ele, int dist);

	// ͬ�ϣ������²�
	static void placeBottom(KvLayoutElement* who, KvLayoutElement* ele, int dist);

	static void align(KcLayoutOverlay* target, KvLayoutElement* ele);

private:

	KuLayoutHelper() = delete;
};

