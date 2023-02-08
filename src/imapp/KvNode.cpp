#include "KvNode.h"
#include "KuStrUtil.h"
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"


KcPortNode::KcPortNode(KeType type, std::weak_ptr<KvBlockNode> parent, unsigned index)
	: type_(type)
	, parent_(parent)
	, index_(index)
	, KvNode(portName_(type, index))
{

}


std::string KcPortNode::portName_(KeType type, unsigned index)
{
	return type == k_in ? "in" + KuStrUtil::toString(index)
		: "out" + KuStrUtil::toString(index);
}


void KvBlockNode::showPropertySet()
{
	ImGui::InputText("Name", &name());
}
