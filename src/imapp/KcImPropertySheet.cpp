#include "KcImPropertySheet.h"
#include "KsImApp.h"
#include "KgImWindowManager.h"
#include "KcImNodeEditor.h"
#include "KgPipeline.h"
#include "imgui.h"


int KcImPropertySheet::flags() const
{
	return KvImWindow::flags() | ImGuiWindowFlags_NoBringToFrontOnFocus;
}


void KcImPropertySheet::updateImpl_()
{
	// 自己不显示东西，调用node的接口显示属性组件

	auto id = KsImApp::singleton().windowManager().getWindow<KcImNodeEditor>()->getSelectedNodeId();

	if (id != -1) {
		auto node = KsImApp::singleton().pipeline().getNode(id);
		auto block = std::dynamic_pointer_cast<KvBlockNode>(node);
		if (block)
			block->showPropertySet();
	}
}
