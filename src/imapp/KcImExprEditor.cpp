#include "KcImExprEditor.h"
#include "imgui.h"
#include "imgui/misc/cpp/imgui_stdlib.h"
#include <memory>
#include "exprtkX/KcExprtk1d.h"
#include "exprtkX/KcExprtk2d.h"
#include "exprtkX/KcExprtk3d.h"
#include "KtContinuedExpr.h"


KcImExprEditor::KcImExprEditor(std::string* text, std::shared_ptr<KvData>* data)
    : exprText_(text)
    , origText_(*text)
    , data_(data)
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
        if (compile_()) {
            origText_.clear(); // 防止重置原始字串
            close();
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


bool KcImExprEditor::compile_()
{
    auto expr = std::make_shared<KcExprtk1d>();
    if (expr->compile(*exprText_)) { // 首先尝试编译一维表达式
        *data_ = std::make_shared<KtContinuedExpr<1>>(expr);
        return true;
    }

    expr = std::make_shared<KcExprtk2d>();
    if (expr->compile(*exprText_)) { // 尝试编译二维表达式
        *data_ = std::make_shared<KtContinuedExpr<2>>(expr);
        return true;
    }

    expr = std::make_shared<KcExprtk3d>();
    if (expr->compile(*exprText_)) { // 尝试编译三维表达式
        *data_ = std::make_shared<KtContinuedExpr<3>>(expr);
        return true;
    }

    return false;
}


void KcImExprEditor::onClose(bool clicked)
{
    super_::onClose(clicked);

    if (!origText_.empty())
        *exprText_ = origText_;
}
