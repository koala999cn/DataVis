#pragma once
#include "KvImWindow.h"


// 
// ����ҳ����
// ���ڶ�̬��ʾѡ��node������
//
class KcImPropertySheet : public KvImWindow
{
public:
	using KvImWindow::KvImWindow;

private:
	void updateImpl_() override;
};