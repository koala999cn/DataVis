#include "KcImPropertySheet.h"
#include "KsImApp.h"
#include "KgImWindowManager.h"
#include "KcImNodeEditor.h"


void KcImPropertySheet::updateImpl_()
{
	// �Լ�����ʾ����������node�Ľӿ���ʾ�������

	auto node = KsImApp::singleton().windowManager().getStatic<KcImNodeEditor>()->selectedNode();
	if (node)
		node->showProperySet();
}
