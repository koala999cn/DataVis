#include "KvNode.h"
#include <string>
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
	return type == k_in ? "in" + std::to_string(index + 1)
		: "out" + std::to_string(index + 1);
}


void KvBlockNode::showPropertySet()
{
	ImGui::InputText("Name", &name());
}
