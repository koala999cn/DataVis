#include "KcImExprEditor.h"
#include "imgui.h"
#include "imgui/misc/cpp/imgui_stdlib.h"


KcImExprEditor::KcImExprEditor(std::string& expr)
    : expr_(expr)
    , KvImModalWindow("ExpressionEditor")
{

}


void KcImExprEditor::updateImpl_()
{
    ImGui::InputText("Expression", &expr_);
    ImGui::Separator();
    if (ImGui::Button("OK")) {
        close();
    }
    ImGui::SameLine();
    ImGui::Spacing();
    if (ImGui::Button("Cancel"))
        close();
}

