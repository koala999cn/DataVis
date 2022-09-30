#pragma once
#include "KvImWindow.h"
#include "KtMatrix4.h"


class KvImGuizmoWindow : public KvImWindow
{
public:

	KvImGuizmoWindow(const std::string_view& name);

	void update() override;

	// 缺省显示ImGuizmo的box例子
	void updateImpl_() override;

protected:
	mat4f viewMat_, projMat_;

private:
	bool noMove_{ false }; // 禁止用户操作trackball的时候，ImGui同时移动窗口
};
