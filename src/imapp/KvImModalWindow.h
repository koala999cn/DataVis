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
	void open();

	// 关闭该模态popup
	void close();
};


