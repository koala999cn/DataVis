#include "KcImActionPanel.h"
#include "imgui.h"
#include "KvAction.h"


void KcImActionPanel::addAction(const std::string_view& groupName, action_ptr act)
{
	groupMaps_[std::string(groupName)].push_back(act);
}


void KcImActionPanel::update()
{
	// ˮƽƽ��button
	const auto btnSize = ImVec2{ ImGui::GetContentRegionAvail().x, 0.0f };

	// ���½���(������İ�ť�б�)�����鴥��action
	for (auto& group : groupMaps_) {
		auto& groupName = group.first;
		auto& actionList = group.second;

		if (ImGui::CollapsingHeader(groupName.c_str())) // �����鴴��header
			for (auto& act : actionList) // ���λ��Ƹ÷����µ�action������ť
				if (ImGui::Button(act->name().c_str(), btnSize)) {
					if (act->trigger())
						triggered_.push_back(act);
				}
	}


	// ���´��ڴ���̬��actions
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
