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
    // �ö��Բ���������Ϊ��ǰ����Ϊ������BeginPopupModal֮�󣬴�ʱpopup-stack�б仯
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
