#pragma once
#include "KvImWindow.h"


class KvImGuizmoWindow : public KvImWindow
{
public:

	using KvImWindow::KvImWindow;

	void update() override;

	void updateImpl_() override {}

private:
	bool noMove_{ false }; // ��ֹ�û�����trackball��ʱ��ImGuiͬʱ�ƶ�����
};
