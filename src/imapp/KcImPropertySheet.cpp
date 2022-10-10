#include "KcImPropertySheet.h"
#include "KsImApp.h"
#include "KgImWindowManager.h"
#include "KcImNodeEditor.h"


void KcImPropertySheet::updateImpl_()
{
	// 自己不显示东西，调用node的接口显示属性组件

	auto node = KsImApp::singleton().windowManager().getStatic<KcImNodeEditor>()->selectedNode();
	if (node)
		node->showProperySet();
}
