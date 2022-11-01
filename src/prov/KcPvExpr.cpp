#include "KcPvExpr.h"
#include "KtContinuedExpr.h"
#include "exprtkX/KcExprtk1d.h"
#include "imgui.h"
#include "imapp/KcImExprEditor.h"
#include "imapp/KsImApp.h"
#include "imapp/KgImWindowManager.h"


KcPvExpr::KcPvExpr()
    : KcPvData("Expression", nullptr)
    , exprText_("sin(x) + 2sin(2x) + 3sin(3x)")
{
    auto exprk = std::make_shared<KcExprtk1d>();
    exprk->compile(exprText_); // ����Ĭ�ϵı��ʽ
    assert(exprk->ok());
    data_ = std::make_shared<KtContinuedExpr<1>>(exprk);
}


void KcPvExpr::showProperySet()
{
    KcPvData::showProperySet();
    
    auto w = ImGui::CalcItemWidth();
    auto sz = ImGui::GetItemRectSize();
    ImGui::Separator();
    if (ImGui::Button("E", ImVec2(sz.y, sz.y))) { // �༭���ʽ�ַ���
        KsImApp::singleton().windowManager().
            registerWindow<KcImExprEditor>(&exprText_, &data_);
    }

    ImGui::PushItemWidth(w - sz.y - ImGui::GetStyle().ItemSpacing.x);
    ImGui::SameLine();
    ImGui::InputText("Formular", exprText_.data(), ImGuiInputTextFlags_ReadOnly);
}
