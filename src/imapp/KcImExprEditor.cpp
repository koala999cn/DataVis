#include "KcImExprEditor.h"
#include "imgui.h"
#include "imgui/misc/cpp/imgui_stdlib.h"


KcImExprEditor::KcImExprEditor(std::string& expr)
    : expr_(expr)
    , KvImModalWindow("Expression Editor")
{

}


int KcImExprEditor::flags() const
{
    return ImGuiWindowFlags_NoSavedSettings;
}


void KcImExprEditor::updateImpl_()
{
    ImGui::PushItemWidth(200);
    ImGui::InputText("Formular", &expr_);
    ImGui::PopItemWidth();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("OK", ImVec2(99, 0))) {
        setVisible(false);
        close_();
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(99, 0))) {
        setVisible(false);
        close_();
    }
}

