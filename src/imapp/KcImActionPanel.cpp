#include "KcImActionPanel.h"
#include "imgui.h"
#include "KvAction.h"
#include "KsImApp.h"
#include "KgPipeline.h"


KcImActionPanel::KcImActionPanel(const std::string_view& name)
	: KvImWindow(name)
{
	minSize_[0] = 120, minSize_[1] = 240;
}


void KcImActionPanel::addAction(const std::string_view& groupName, action_ptr act)
{
	groupMaps_[std::string(groupName)].push_back(act);
}


int KcImActionPanel::flags() const
{
	return KvImWindow::flags() | ImGuiWindowFlags_NoBringToFrontOnFocus;
}


void KcImActionPanel::updateImpl_()
{
	// 根据节点类型，配置r、g、b三个色系
	unsigned int clrTitleBar[] = {
		IM_COL32(191, 11, 109, 255),
		IM_COL32(11, 191, 109, 255),
		IM_COL32(11, 109, 191, 255)
	};

	unsigned int clrTitleBarSelected[] = {
		IM_COL32(204, 81, 148, 255),
		IM_COL32(81, 204, 148, 255),
		IM_COL32(81, 148, 204, 255)
	};

	unsigned int clrTitleBarHovered[] = {
		IM_COL32(194, 45, 126, 255),
		IM_COL32(45, 194, 126, 255),
		IM_COL32(45, 126, 194, 255)
	};

	// 水平平铺button
	const auto btnSize = ImVec2{ ImGui::GetContentRegionAvail().x, 0.0f };

	bool disable = KsImApp::singleton().pipeline().running();
	
	// 更新界面(被分组的按钮列表)并视情触发action
	int i = 0;
	for (auto& group : groupMaps_) {
		auto& groupName = group.first;
		auto& actionList = group.second;

		// TOOD: 更通用的实现
		ImGui::PushStyleColor(ImGuiCol_Header, clrTitleBar[i % 3]);
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, clrTitleBarSelected[i % 3]);
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, clrTitleBarHovered[i % 3]);
		++i;

		// 按分组创建header，第1组默认打开
		if (ImGui::CollapsingHeader(groupName.c_str(), i == 1 ? ImGuiTreeNodeFlags_DefaultOpen : 0)) {
			ImGui::BeginDisabled(disable);

			for (auto& act : actionList) // 依次绘制该分组下的action触发按钮
				if (ImGui::Button(act->name().c_str(), btnSize)) {
					if (!act->triggered() && act->trigger())
						triggered_.push_back(act);
				}

			ImGui::EndDisabled();
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}


	// 更新处于触发态的actions
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
