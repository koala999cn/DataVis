#include "KcPvFunction.h"
#include "dsp/KcContinuedFn.h"
#include "dsp/KtSampledExpr.h"
#include "exprtkX/KcExprtk2d.h"
#include "imguix.h"


KcPvFunction::KcPvFunction()
    : super_("Function", nullptr)
    , exprs_(3) 
{
    init_();
}


void KcPvFunction::init_()
{
    if (dim_ == 2) {
        exprs_[0] = "sin(x) * cos(y)";
        exprs_[1] = "sin(x) * sin(y)";
        exprs_[2] = "cos(x)";

        iranges_[0] = { 0.f, static_cast<float>(KuMath::pi) };
        iranges_[1] = { 0.f, static_cast<float>(KuMath::pi * 2) };

        counts_[0] = counts_[1] = 32;

        std::array<std::shared_ptr<KvContinued>, 3> ptrs;

        for (int i = 0; i < 3; i++) {
            auto expr = std::make_shared<KcExprtk2d>();
            expr->compile(exprs_[i]);
            assert(expr->ok());
            ptrs[i] = std::make_shared<KcContinuedFn>(
                [expr](kReal x[]) { return expr->value(x); }, 1);
        }

        auto d = std::make_shared<KtSampledExpr<2>>(ptrs);
        setData(d);
    }
    else {
        exprs_[0] = "sin(x)";
        exprs_[1] = "cos(x)";

        iranges_[0] = { -static_cast<float>(KuMath::pi), static_cast<float>(KuMath::pi) };

        counts_[0] = 1000;

        std::array<std::shared_ptr<KvContinued>, 2> ptrs;

        for (int i = 0; i < 2; i++) {
            auto expr = std::make_shared<KcExprtk1d>();
            expr->compile(exprs_[i]);
            assert(expr->ok());
            ptrs[i] = std::make_shared<KcContinuedFn>(
                [expr](kReal x[]) { return expr->value(x); }, 1);
        }

        auto d = std::make_shared<KtSampledExpr<1>>(ptrs);
        setData(d);
    }

    updateDataSamplings_();
}


void KcPvFunction::updateDataSamplings_()
{
    auto samp = std::dynamic_pointer_cast<KvSampled>(data());

    for (int i = 0; i < dim_; i++)
        samp->reset(i, iranges_[i].first, (iranges_[i].second - iranges_[i].first) / (counts_[i] - 1));

    kIndex shape[2]{ counts_[0], counts_[1] };
    samp->resize(shape, 0);

    notifyChanged();
}


void KcPvFunction::showPropertySet()
{
    super_::showPropertySet();
    ImGui::Separator();

    auto d = data();

    if (ImGui::SliderInt("Dim", &dim_, 1, 2))
        init_();

    if (ImGuiX::treePush("Expressions", true)) {
        for (unsigned i = 0; i <= dim_; i++) {
            std::string label = "f1";
            label.back() += i;
            ImGui::PushID(i);
            ImGuiX::exprEdit(label.c_str(), exprs_[i].c_str(), dim_,
                [this, i, d](std::shared_ptr<KvData> data, const char* text) {
                    auto expr = std::dynamic_pointer_cast<KvContinued>(data);
                    assert(expr);

                    exprs_[i] = text;     

                    if (dim_ == 2) 
                        std::dynamic_pointer_cast<KtSampledExpr<2>>(d)->setExpr(i, expr);
                    else
                        std::dynamic_pointer_cast<KtSampledExpr<1>>(d)->setExpr(i, expr);
                });
            ImGui::PopID();
        }
        ImGuiX::treePop();
    }

    if (ImGuiX::treePush("Sampling", true)) {
        for (unsigned i = 0; i < dim_; i++) {
            std::string label = "RangeX";
            label.back() += i;      
            float low(iranges_[i].first), high(iranges_[i].second);
            if (ImGui::DragFloatRange2(label.c_str(), &low, &high) && high > low) {
                iranges_[i].first = low, iranges_[i].second = high;
                updateDataSamplings_();
            }
        }

        int cmin(2), cmax(1024);
        if (ImGui::DragScalarN("Count", ImGuiDataType_S32, counts_, dim_, 1, &cmin, &cmax, "%d")) {
            if (counts_[0] < 2) counts_[0] = 2;
            if (counts_[1] < 2) counts_[1] = 2;
            updateDataSamplings_();
        }

        ImGuiX::treePop();
    }
}


kRange KcPvFunction::range(kIndex outPort, kIndex axis) const
{
    return axis < 2 ? oranges_[axis] : super_::range(outPort, axis);
}


void KcPvFunction::notifyChanged(unsigned outPort)
{
    oranges_[0] = data() ? data()->range(0) : kRange(0, 0);
    oranges_[1] = data() ? data()->range(1) : kRange(0, 0);

    super_::notifyChanged(outPort);
}
