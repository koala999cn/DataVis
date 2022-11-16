#include "KcOpInterpolater.h"
#include "dsp/KcInterpolater.h"
#include "KvDiscreted.h"
#include "imgui.h"


KcOpInterpolater::KcOpInterpolater()
    : super_("Interpolate")
{

}


int KcOpInterpolater::spec(kIndex outPort) const
{
    KpDataSpec sp(super_::spec(outPort));
    sp.type = k_continued;
    return sp.spec;
}


kReal KcOpInterpolater::step(kIndex outPort, kIndex axis) const
{
    assert(axis < dim(outPort));
    return 0;
}


kIndex KcOpInterpolater::size(kIndex outPort, kIndex axis) const
{
    assert(axis < dim(outPort));
    return KvData::k_inf_size;
}


bool KcOpInterpolater::permitInput(int dataSpec, unsigned inPort) const
{
    assert(inPort == 0);
    KpDataSpec sp(dataSpec);
    return sp.type != k_continued && sp.dim == 1;
}


void KcOpInterpolater::showProperySet()
{
    super_::showProperySet();
    ImGui::Separator();

    static const char* interp[] = { "Linear", "Quad" };
    if (ImGui::BeginCombo("Interpolate Method", interp[interpMethod_])) {
        for (unsigned i = 0; i < std::size(interp); i++)
            if (ImGui::Selectable(interp[i], i == interpMethod_))
                interpMethod_ = i;
        ImGui::EndCombo();
    }


    static const char* extrap[] = {
        "Nan",
        "Zero", 
        "Const",
        "Mirro", 
        "Period",
        "Extrap"
    };
    if (ImGui::BeginCombo("Extrapolate Method", extrap[extrapMethod_])) {
        for (unsigned i = 0; i < std::size(extrap); i++)
            if (ImGui::Selectable(extrap[i], i == extrapMethod_))
                extrapMethod_ = i;
        ImGui::EndCombo();
    }
}


void KcOpInterpolater::output()
{
    if (idata_.front()) {
        auto disc = std::dynamic_pointer_cast<KvDiscreted>(idata_.front());
        auto interp = std::make_shared<KcInterpolater>(disc);
        interp->setInterMethod(interpMethod_);
        interp->setExtraMethod(extrapMethod_);
        odata_.front() = interp;
    }
    else {
        odata_.front() = nullptr;
    }
}