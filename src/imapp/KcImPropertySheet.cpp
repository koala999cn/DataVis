#include "KcImPropertySheet.h"
#include "KsImApp.h"
#include "KgImWindowManager.h"
#include "KcImNodeEditor.h"
#include "imgui.h"


int KcImPropertySheet::flags() const
{
	return ImGuiWindowFlags_NoBringToFrontOnFocus;
}


void KcImPropertySheet::updateImpl_()
{
	// �Լ�����ʾ����������node�Ľӿ���ʾ�������

	auto node = KsImApp::singleton().windowManager().getStatic<KcImNodeEditor>()->selectedNode();
	if (node)
		node->showProperySet();
}
