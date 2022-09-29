#pragma once
#include "KvImWindow.h"


class KvImGuizmoWindow : public KvImWindow
{
public:

	using KvImWindow::KvImWindow;

	void update() override;

	void updateImpl_() override {}

private:
	bool noMove_{ false }; // 禁止用户操作trackball的时候，ImGui同时移动窗口
};
