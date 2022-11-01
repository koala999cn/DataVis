#include "KvImModalWindow.h"
#include "imgui.h"
#include "imgui_internal.h"
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

    // Always center this window when appearing
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    ImGui::SetNextWindowSizeConstraints(
        ImVec2{ minSize_[0], minSize_[1] },
        ImVec2{ maxSize_[0], maxSize_[1] }
    );

    if (!opened_()) 
        openPopup_();

    if (ImGui::BeginPopupModal(label().c_str(), &visible_, flags())) {

        // NOTE: 以下这行代码很重要！解决了ImGui显示Modal窗口的一个问题
        // 在弹出模式窗口情况下，最下化主窗口再恢复，模式窗口会消失(如果不再open)；
        // 或显示在其他窗口后面(始终reopen)，而这时程序整体处于Modal状态，将不可用。
        // 参考https://github.com/ocornut/imgui/issues/1328
        // TODO: 使用这个方案，会导致combo无法正常弹出下拉框
        // ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow()); // needs imgui_internal.h

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


std::string KvImModalWindow::label() const
{
    return name();
}
