#include "KcPvExpr.h"
#include "KcContinuedFn.h"
#include "exprtkX/KcExprtk1d.h"
#include "imgui.h"
#include "imapp/KcImExprEditor.h"
#include "imapp/KsImApp.h"
#include "imapp/KgImWindowManager.h"
#include "imapp/KgPipeline.h"


KcPvExpr::KcPvExpr()
    : KcPvData("Expression", nullptr)
    , exprText_("sin(x) + 2sin(2x) + 3sin(3x)")
{
    auto expr = std::make_shared<KcExprtk1d>();
    expr->compile(exprText_); // 构造默认的表达式
    assert(expr->ok());

    auto cont = std::make_shared<KcContinuedFn>(
        [expr](kReal x[]) { return expr->value(x); }, 1);

    setData(cont);
}


void KcPvExpr::showProperySet()
{
    KcPvData::showProperySet();
    
    auto w = ImGui::CalcItemWidth();
    auto sz = ImGui::GetItemRectSize();
    ImGui::Separator();
    bool disable = KsImApp::singleton().pipeline().running();
    ImGui::BeginDisabled(disable);

    if (ImGui::Button("E", ImVec2(sz.y, sz.y))) { // 编辑表达式字符串
        KsImApp::singleton().windowManager().
            registerWindow<KcImExprEditor>(&exprText_, this);
    }
    ImGui::EndDisabled();

    ImGui::PushItemWidth(w - sz.y - ImGui::GetStyle().ItemSpacing.x);
    ImGui::SameLine();

    ImGui::BeginDisabled();
    ImGui::InputText("Formular", exprText_.data(), ImGuiInputTextFlags_ReadOnly);
    ImGui::EndDisabled();
}
