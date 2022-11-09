#include "KcImExprEditor.h"
#include "imgui.h"
#include "imgui/misc/cpp/imgui_stdlib.h"
#include <memory>
#include "exprtkX/KcExprtk1d.h"
#include "exprtkX/KcExprtk2d.h"
#include "exprtkX/KcExprtk3d.h"
#include "KcContinuedFn.h"
#include "prov/KcPvData.h"


KcImExprEditor::KcImExprEditor(std::string* text, KcPvData* pvData)
    : exprText_(text)
    , origText_(*text)
    , pvData_(pvData)
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
    ImGui::InputText("Formular", exprText_);
    ImGui::SetItemDefaultFocus();
    ImGui::PopItemWidth();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("OK", ImVec2(99, 0))) {
        auto data = compile_();
        if (data) {
            origText_.clear(); // 防止重置原始字串
            close();
            pvData_->setData(data);
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
    auto expr = std::make_shared<KcExprtk1d>();
    if (expr->compile(*exprText_))  // 首先尝试编译一维表达式
        return std::make_shared<KcContinuedFn>(
            [expr](kReal x[]) { return expr->value(x); }, 
            1);

    expr = std::make_shared<KcExprtk2d>();
    if (expr->compile(*exprText_))  // 尝试编译二维表达式
        return std::make_shared<KcContinuedFn>(
            [expr](kReal x[]) { return expr->value(x); },
            2);

    expr = std::make_shared<KcExprtk3d>();
    if (expr->compile(*exprText_))  // 尝试编译三维表达式
        return std::make_shared<KcContinuedFn>(
            [expr](kReal x[]) { return expr->value(x); },
            3);

    return nullptr;
}


void KcImExprEditor::onClose(bool clicked)
{
    super_::onClose(clicked);

    if (!origText_.empty())
        *exprText_ = origText_;
}
