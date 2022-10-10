#pragma once
#include "KvImWindow.h"


// 
// 属性页窗口
// 用于动态显示选中node的属性
//
class KcImPropertySheet : public KvImWindow
{
public:
	using KvImWindow::KvImWindow;

private:
	void updateImpl_() override;
};