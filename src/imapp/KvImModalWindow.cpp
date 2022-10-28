#include "KvImModalWindow.h"
#include "imgui.h"
#include <assert.h>


KvImModalWindow::KvImModalWindow(const std::string_view& _name)
    : KvImWindow(_name)
{
    deleteOnClose_ = true;
}


KvImModalWindow::~KvImModalWindow()
{
    assert(!opened_());
}


void KvImModalWindow::update()
{
    assert(visible());

    if (!opened_())
        openPopup_();

    // Always center this window when appearing
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal(label().c_str(), &visible_, flags())) {
        updateImpl_();
        ImGui::EndPopup();
    }
    else if (!visible_) {
        onClose(true);
    }
}


bool KvImModalWindow::opened_() const
{
    return ImGui::IsPopupOpen(label().c_str());
}


void KvImModalWindow::openPopup_()
{
    assert(!opened_());
    ImGui::OpenPopup(label().c_str());
    assert(opened_());
}


void KvImModalWindow::closePopup_()
{
    // 该断言不成立，因为当前环境为调用了BeginPopupModal之后，此时popup-stack有变化
    //if(opened_())
        ImGui::CloseCurrentPopup();
}


void KvImModalWindow::onClose(bool clicked)
{
    if (!clicked)
        closePopup_();
}
