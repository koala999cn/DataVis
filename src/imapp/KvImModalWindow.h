#pragma once
#include "KvImWindow.h"


// 模态窗口基类
class KvImModalWindow : public KvImWindow
{
public:
	KvImModalWindow(const std::string_view& name);
	KvImModalWindow(std::string&& name);
	virtual ~KvImModalWindow();

	void update() override;

	bool opened() const;

	// 打开该模态popup，只有打开了popup的模态窗台才update
	// 须由用户显示调用
	void open();

protected:

	// 关闭该模态popup
	// 须在EndPopup之前调用，即由用户在updateImpl_实现中调用
	void close();
};


