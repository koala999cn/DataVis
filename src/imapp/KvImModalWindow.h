#pragma once
#include "KvImWindow.h"


// 模态窗口基类
class KvImModalWindow : public KvImWindow
{
public:
	KvImModalWindow(const std::string_view& name);
	virtual ~KvImModalWindow();

	void update() override;

	void onClose(bool clicked) override;

	std::string label() const override;

private:

	// 打开该模态popup，只有打开了popup的模态窗台才update
	// 用户可显示调用，update方法也会在必要的时候调用该函数
	void openPopup_();

	bool opened_() const;

	// 关闭该模态popup
	// 须在EndPopup之前调用，即由用户在updateImpl_实现中调用
	void closePopup_();
};


