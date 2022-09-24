#include "KcImActionList.h"
#include "imgui.h"
#include "ImFileDialog/ImFileDialog.h"
#include "KcActionLoadText.h"


KcImActionList::KcImActionList(const std::string_view& name)
	: KvImWindow(name)
{
	providers_.push_back(std::make_shared<KcActionLoadText>("Text Data"));
}


void KcImActionList::draw()
{
	if (ImGui::CollapsingHeader("Provider"))
		drawActionButtons_(providers_);

	if (ImGui::CollapsingHeader("Operator")) {

	}

	if (ImGui::CollapsingHeader("Renderer")) {

	}

	auto iter = triggered_.begin();
	while (iter != triggered_.end()) {
		auto& act = *iter;
		if (act->triggered()) {
			act->update();
			iter++;
		}
		else {
			iter = triggered_.erase(iter);
		}
	}
}


void KcImActionList::drawActionButtons_(std::vector<action_ptr>& actions)
{
	// Ë®Æ½Æ½ÆÌbutton
	const auto btnSize = ImVec2{ ImGui::GetContentRegionAvail().x, 0.0f };

	for (auto& a : actions)
		if (ImGui::Button(a->name().c_str(), btnSize))
			trigger_(a);
}


void KcImActionList::trigger_(action_ptr act)
{
	if (act->trigger())
		triggered_.push_back(act);
}