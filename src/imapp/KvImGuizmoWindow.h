#pragma once
#include "KvImWindow.h"
#include "KtMatrix4.h"


class KvImGuizmoWindow : public KvImWindow
{
public:

	KvImGuizmoWindow(const std::string_view& name);

	void update() override;

	// ȱʡ��ʾImGuizmo��box����
	void updateImpl_() override;

protected:
	mat4f viewMat_, projMat_;

private:
	bool noMove_{ false }; // ��ֹ�û�����trackball��ʱ��ImGuiͬʱ�ƶ�����
};
