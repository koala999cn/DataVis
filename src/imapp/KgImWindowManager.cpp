#include "KgImWindowManager.h"
#include "KvImWindow.h"
#include "imgui.h"
#include <assert.h>


KgImWindowManager::KgImWindowManager()
{

}


KgImWindowManager::~KgImWindowManager()
{
	releaseAll();
}


void KgImWindowManager::registerStatic(window_ptr inst)
{
	assert(getStatic(inst->name()) == nullptr);
	statics_.push_back(inst);
}


void KgImWindowManager::registerDynamic(window_ptr inst)
{
	registerQueue_.push_back(inst);
}


KgImWindowManager::window_ptr KgImWindowManager::getStatic(const std::string_view& name)
{
	for (auto& i : statics_)
		if (i->name() == name)
			return i;

	return {};
}


void KgImWindowManager::releaseStatic(window_ptr inst)
{
	auto iter = statics_.begin();
	for (; iter != statics_.end(); iter++)
		if (*iter == inst) {
			statics_.erase(iter);
			break;
		}
}


void KgImWindowManager::releaseStatic(const std::string_view& name)
{
	auto iter = statics_.begin();
	for (; iter != statics_.end(); iter++)
		if ((*iter)->name() == name) {
			statics_.erase(iter);
			break;
		}
}


void KgImWindowManager::releaseDynamic(window_ptr inst)
{
	releaseQueue_.push_back(inst);
}


void KgImWindowManager::showMenu(const std::string_view& menuName)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 10.0f, 10.0f });

    if (ImGui::BeginMenu(menuName.data())) {

		// Style
		if (ImGui::BeginMenu("Style")) {
			if (ImGui::MenuItem("Dark", ""))
				ImGui::StyleColorsDark();
			if (ImGui::MenuItem("Classic", ""))
				ImGui::StyleColorsClassic();
			if (ImGui::MenuItem("Light", ""))
				ImGui::StyleColorsLight();
			ImGui::EndMenu();
		}

		ImGui::Separator();

		// 主视图
		for (auto& w : statics_) {
			bool selected = w->visible();
			if (ImGui::MenuItem(w->name().data(), "", &selected))
				w->setVisible(selected);
		}

		ImGui::Separator();

		// ImGui内置窗口
        if (ImGui::BeginMenu("ImGui")) {
            ImGui::MenuItem("Demo", "", &showDemo_);
			ImGui::MenuItem("Debug Log", "", &showDebugLog_);
            ImGui::MenuItem("Metrics/Debugger", "", &showMetrics_);
            ImGui::MenuItem("Stack Tool", "", &showStackTool_);
			ImGui::MenuItem("About", "", &showAbout_);
            ImGui::EndMenu();
        }

        ImGui::Separator();

		if (!dynamics_.empty()) {

			// 动态窗口
			if (ImGui::BeginMenu("Windows")) {
				for (auto& w : dynamics_) {
					bool selected = w->visible();
					if (ImGui::MenuItem(w->name().data(), "", &selected))
						w->setVisible(selected);
				}
				ImGui::EndMenu();
			}

			ImGui::Separator();
		}

        if (ImGui::MenuItem("Close All"))
            closeAll();
        if (ImGui::MenuItem("Show All"))
            showAll();

        ImGui::EndMenu();
    }

    ImGui::PopStyleVar();
}


void KgImWindowManager::update()
{
	for(auto i = registerQueue_.begin(); i != registerQueue_.end(); i++)
		dynamics_.push_back(*i);
	registerQueue_.clear();

	for (auto i = releaseQueue_.begin(); i != releaseQueue_.end(); i++) {
		auto pos = std::find(dynamics_.begin(), dynamics_.end(), *i);
		if (pos != dynamics_.end())
			dynamics_.erase(pos);
	}
	releaseQueue_.clear();

	for (auto& w : statics_)
		if(w->visible())
			w->update();

	for (auto& w : dynamics_)
		if (w->visible())
			w->update();

	if (showDemo_)
		ImGui::ShowDemoWindow(&showDemo_);
	if (showDebugLog_)
		ImGui::ShowDebugLogWindow(&showDebugLog_);
	if (showMetrics_)
		ImGui::ShowMetricsWindow(&showMetrics_);
	if (showStackTool_)
	    ImGui::ShowStackToolWindow(&showStackTool_);
	if (showAbout_)
		ImGui::ShowAboutWindow(&showAbout_);
}


void KgImWindowManager::setVisible_(bool b)
{
	for (auto& w : statics_)
		w->setVisible(b);

	for (auto& w : dynamics_)
		w->setVisible(b);
	if (!b) dynamics_.clear();

	// 仅当关闭窗口的时候，作用到imgui内置窗口
	if (!b) {
		showDemo_ = b;
		showMetrics_ = b;
		showDebugLog_ = b;
		showStackTool_ = b;
		showAbout_ = b;
	}
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
	registerQueue_.clear();
	releaseQueue_.clear();
	dynamics_.clear();
	statics_.clear();
}
