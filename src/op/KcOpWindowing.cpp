#include "KcOpWindowing.h"
#include "dsp/KgWindowing.h"
#include "imgui.h"


KcOpWindowing::KcOpWindowing()
    : super_("Windowing", true, true)
    , type_(KgWindowing::k_hamming)
    , arg_(0)
{

}


bool KcOpWindowing::onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins)
{
    assert(win_ == nullptr);
    assert(isize_() > 0);

    if (!super_::onStartPipeline(ins))
        return false;

    prepareOutput_();
    createOutputData_();
    return win_ != nullptr && odata_.front() != nullptr;
}


void KcOpWindowing::onStopPipeline()
{
    win_.reset();
    super_::onStopPipeline();
}


void KcOpWindowing::showPropertySet()
{
    super_::showPropertySet();
    ImGui::Separator();

    if (ImGui::BeginCombo("Type", KgWindowing::type2Str(KgWindowing::KeType(type_)))) {
        for (unsigned i = 0; i < KgWindowing::k_type_count; i++)
            if (ImGui::Selectable(KgWindowing::type2Str(KgWindowing::KeType(i)), i == type_)) {
                type_ = i;
                setOutputExpired(0);
            }
        ImGui::EndCombo();
    }

    if (ImGui::InputFloat("Arg", &arg_))
        setOutputExpired(0);
}


bool KcOpWindowing::prepareOutput_()
{
    auto isize = isize_();
    if (isOutputExpired() || !win_ || win_->idim() != isize) {
        win_ = std::make_unique<KgWindowing>(isize, KgWindowing::KeType(type_), arg_);
        return true;
    }

    return false;
}


void KcOpWindowing::op_(const kReal* in, unsigned len, kReal* out)
{
    assert(len == win_->idim());
    win_->process(in, out);
}
