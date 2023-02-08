#include "KcImExprEditor.h"
#include "imgui.h"
#include "imgui/misc/cpp/imgui_stdlib.h"
#include <memory>
#include "exprtkX/KcExprtk1d.h"
#include "exprtkX/KcExprtk2d.h"
#include "exprtkX/KcExprtk3d.h"
#include "KcContinuedFn.h"
#include "prov/KcPvData.h"


KcImExprEditor::KcImExprEditor(const char* text, unsigned dim, handler_t h)
    : text_(text)
    , handler_(h)
    , dim_(dim)
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
    ImGui::InputText("Formular", &text_);
    ImGui::SetItemDefaultFocus();
    ImGui::PopItemWidth();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("OK", ImVec2(99, 0))) {
        auto data = compile_();
        if (data) {
            close();
            handler_(data, text_.c_str());
        }
        else {
            // TODO: messagebox
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(99, 0))) {
        close();
    }
}


std::shared_ptr<KvData> KcImExprEditor::compile_()
{
    if (dim_ == 0 || dim_ == 1) {
        auto expr = std::make_shared<KcExprtk1d>();
        if (expr->compile(text_))  // 首先尝试编译一维表达式
            return std::make_shared<KcContinuedFn>(
                [expr](kReal x[]) { return expr->value(x); },
                1);
    }

    if (dim_ == 0 || dim_ == 2) {
        auto expr = std::make_shared<KcExprtk2d>();
        if (expr->compile(text_))  // 尝试编译二维表达式
            return std::make_shared<KcContinuedFn>(
                [expr](kReal x[]) { return expr->value(x); },
                2);
    }

    if (dim_ == 0 || dim_ == 3) {
        auto expr = std::make_shared<KcExprtk3d>();
        if (expr->compile(text_))  // 尝试编译三维表达式
            return std::make_shared<KcContinuedFn>(
                [expr](kReal x[]) { return expr->value(x); },
                3);
    }

    return nullptr;
}
