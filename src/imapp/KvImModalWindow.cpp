#include "KvImModalWindow.h"
#include "imgui.h"


KvImModalWindow::KvImModalWindow(const std::string_view& _name)
    : KvImWindow(_name)
{
    open(); // TODO: �и��õĵط�������
}


KvImModalWindow::KvImModalWindow(std::string&& _name)
    : KvImWindow(std::move(_name))
{
    open();
}


void KvImModalWindow::update()
{
    if (opened()) {

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        bool opend{ true };
        if (ImGui::BeginPopupModal(name().c_str(), &opend, flags())) {
            updateImpl_();
            ImGui::EndPopup();
        }

        if (!opend) // �û�����˹رհ�ť
            close();
    }
}


bool KvImModalWindow::opened() const
{
    return ImGui::IsPopupOpen(name().c_str());
}


void KvImModalWindow::open()
{
    ImGui::OpenPopup(name().c_str());
}


void KvImModalWindow::close()
{
    ImGui::CloseCurrentPopup();
    setVisible(false); // TODO: ��δ���open��visible�Ĺ�ϵ
}