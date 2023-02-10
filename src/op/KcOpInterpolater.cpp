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


void KcOpInterpolater::showPropertySet()
{
    super_::showPropertySet();
    ImGui::Separator();

    static const char* interp[] = { "Linear", "Quad" };
    if (ImGui::Combo("Interpolate Method", &interpMethod_, interp, std::size(interp)))
        setOutputExpired(0);

    static const char* extrap[] = {
        "Nan",
        "Zero", 
        "Const",
        "Mirro", 
        "Period",
        "Extrap"
    };
    if (ImGui::Combo("Extrapolate Method", &extrapMethod_, extrap, std::size(extrap)))
        setOutputExpired(0);
}


void KcOpInterpolater::outputImpl_()
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