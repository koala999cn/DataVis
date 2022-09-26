#include "KgImWindowManager.h"
#include "KvImWindow.h"
#include "imgui.h"


KgImWindowManager::KgImWindowManager()
{
	// 向缺省分组插入一个空元素，确保缺省分组排在最前面（unorder_map）
	groups_.insert({ "_default_", window_group() });
}


KgImWindowManager::~KgImWindowManager()
{
	releaseAll();
}


void KgImWindowManager::registerInstance(window_ptr instance)
{
	registerInstance(instance, "_default_");
}


void KgImWindowManager::registerInstance(window_ptr instance, const std::string_view& group)
{
	auto& g = groups_[std::string(group)];
	g.push_back(instance);
}


void KgImWindowManager::releaseInstance(window_ptr instance)
{
	for (auto& g : groups_)
		for (auto i = g.second.begin(); i != g.second.end(); i++)
			if (*i == instance)
				g.second.erase(i);
}


void KgImWindowManager::releaseInstance(const std::string_view& name)
{
	for (auto& g : groups_)
		for (auto i = g.second.begin(); i != g.second.end(); i++)
			if (i->get()->name() == name)
				g.second.erase(i);
}


void KgImWindowManager::showMenu(const std::string_view& menuName)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 10.0f, 10.0f });

    if (ImGui::BeginMenu(menuName.data())) {
		for (auto& g : groups_) {
			for (auto i = g.second.begin(); i != g.second.end(); i++) {
				auto& w = *i;
				bool selected = w->visible();
				if (ImGui::MenuItem(w->name().data(), "", &selected))
					w->setVisible(selected);
			}

			ImGui::Separator();
		}

        if (ImGui::BeginMenu("ImGui")) {
            ImGui::MenuItem("Demo", "", &showDemo_);
			ImGui::MenuItem("Debug Log", "", &showDebugLog_);
            ImGui::MenuItem("Metrics/Debugger", "", &showMetrics_);
            ImGui::MenuItem("Stack Tool", "", &showStackTool_);
            ImGui::EndMenu();
        }

        ImGui::Separator();
        if (ImGui::MenuItem("Close All"))
            closeAll();
        if (ImGui::MenuItem("Show All"))
            showAll();

        ImGui::EndMenu();
    }

    ImGui::PopStyleVar();
}


void KgImWindowManager::draw()
{
	for (auto& g : groups_)
		for (auto i = g.second.begin(); i != g.second.end(); i++) {
			auto& w = *i;
			if (w->visible()) {
				if (w->begin())
				    w->update();
				w->end();
			}
		}	

	if (showDemo_)
		ImGui::ShowDemoWindow(&showDemo_);
	if (showDebugLog_)
		ImGui::ShowDebugLogWindow(&showDebugLog_);
	if (showMetrics_)
		ImGui::ShowMetricsWindow(&showMetrics_);
	if (showStackTool_)
	    ImGui::ShowStackToolWindow(&showStackTool_);
}


void KgImWindowManager::setVisible_(bool b)
{
	for (auto& g : groups_)
		for (auto& w : g.second)
			w->setVisible(b);

	showDemo_ = b;
	showMetrics_ = b;
	showDebugLog_ = b;
	showStackTool_ = b;
}


void KgImWindowManager::showAll()
{
	setVisible_(true);
}


void KgImWindowManager::closeAll()
{
	setVisible_(false);
}


void KgImWindowManager::releaseAll()
{
	groups_.clear();
}
