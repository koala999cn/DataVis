#include "KcPvExpr.h"
#include "KcContinuedFn.h"
#include "exprtkX/KcExprtk1d.h"
#include "imguix.h"
#include "imapp/KsImApp.h"
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
    ImGui::Separator();

    ImGui::BeginDisabled(KsImApp::singleton().pipeline().running());

    ImGuiX::exprEdit("Formular", exprText_.c_str(), 0, 
        [this](std::shared_ptr<KvData> data, const char* text) {
            exprText_ = text;
            setData(data); 
        });

    ImGui::EndDisabled();
}
