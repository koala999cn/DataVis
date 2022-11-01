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


void KgImWindowManager::registerWindow(window_ptr inst)
{
	// assert(getWindow(inst->name()) == nullptr);
	registerQueue_.push_back(inst);
}


KgImWindowManager::window_ptr KgImWindowManager::getWindow(const std::string_view& name)
{
	for (auto& i : winlist_)
		if (i->name() == name)
			return i;

	return {};
}

void KgImWindowManager::releaseWindow(window_ptr inst)
{
	releaseQueue_.push_back(inst);
}


void KgImWindowManager::releaseWindow(const std::string_view& label)
{
	for (auto iter = winlist_.begin(); iter != winlist_.end(); iter++)
		if ((*iter)->label() == label) {
			releaseQueue_.push_back(*iter);
			break;
		}
}


void KgImWindowManager::releaseWindow(int id)
{
	for (auto iter = winlist_.begin(); iter != winlist_.end(); iter++)
		if ((*iter)->id() == id) {
			releaseQueue_.push_back(*iter);
			break;
		}
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
		for (auto& w : winlist_) {
			if (!w->dynamic()) { // 只显示非dynamic窗口
				bool selected = w->visible();
				if (ImGui::MenuItem(w->name().data(), "", &selected))
					w->setVisible(selected);
			}
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

		//if (!dynamics_.empty()) {

			// 动态窗口
			if (ImGui::BeginMenu("Windows")) {
				for (auto& w : winlist_) {
					if (w->dynamic()) {
						bool selected = w->visible();
						if (ImGui::MenuItem(w->name().data(), "", &selected))
							w->setVisible(selected);
					}
				}
				ImGui::EndMenu();
			}

			ImGui::Separator();
		//}

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
		winlist_.push_back(*i);
	registerQueue_.clear();

	for (auto i = releaseQueue_.begin(); i != releaseQueue_.end(); i++) {
		auto pos = std::find(winlist_.begin(), winlist_.end(), *i);
		if (pos != winlist_.end())
			winlist_.erase(pos);
	}
	releaseQueue_.clear();

	for (auto& w : winlist_)
		if (w->visible())
			w->update();
		else if (w->deleteOnClose())
			releaseQueue_.push_back(w);


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


void KgImWindowManager::setVisibleAll_(bool b)
{
	for (auto& w : winlist_)
		w->setVisible(b);


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
	setVisibleAll_(true);
}


void KgImWindowManager::closeAll()
{
	setVisibleAll_(false);
}


void KgImWindowManager::releaseAll()
{
	registerQueue_.clear();
	releaseQueue_.clear();
	winlist_.clear();
}
