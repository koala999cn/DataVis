#pragma once
#include "KcActionSequence.h"


// LoadText����: ��3���Ӷ�������
// һ��KcOpenFileAction�������Ի���ѡȡ�ļ�
// ����KcDataFormatAction�����û�ѡȡ��������
// ����KcNewNodeAction����NodeEditor����provider�ڵ�

class KcActionLoadText : public KcActionSequence
{
public:
	explicit KcActionLoadText();
};
