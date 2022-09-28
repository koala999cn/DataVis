#include "KvImModalWindow.h"
#include "imgui.h"


KvImModalWindow::KvImModalWindow(const std::string_view& _name)
    : KvImWindow(_name)
{
    open(); // TODO: 有更好的地方调用吗？
}


KvImModalWindow::KvImModalWindow(std::string&& _name)
    : KvImWindow(std::move(_name))
{
    open();
}


void KvImModalWindow::update()
{
    assert(opened());
    assert(visible());

    // Always center this window when appearing
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal(name().c_str(), &visible_, flags())) {
        updateImpl_();
        ImGui::EndPopup();
    }
}


bool KvImModalWindow::opened() const
{
    return ImGui::IsPopupOpen(name().c_str());
}


void KvImModalWindow::open()
{
    assert(!opened());
    ImGui::OpenPopup(name().c_str());
    assert(opened());
}


void KvImModalWindow::close()
{
    if (opened()) 
        ImGui::CloseCurrentPopup();
}