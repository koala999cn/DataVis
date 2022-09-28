#include "KvImModalWindow.h"
#include "imgui.h"
#include <assert.h>


KvImModalWindow::KvImModalWindow(const std::string_view& _name)
    : KvImWindow(_name)
{

}


KvImModalWindow::KvImModalWindow(std::string&& _name)
    : KvImWindow(std::move(_name))
{

}


KvImModalWindow::~KvImModalWindow()
{
    assert(!opened());
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
        if (!visible())
            close();
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
    // 该断言不成立，因为当前环境为调用了BeginPopupModal之后，此时popup-stack有变化
    //assert(opened()); 

    ImGui::CloseCurrentPopup();
}
