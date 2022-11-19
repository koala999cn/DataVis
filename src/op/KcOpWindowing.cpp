#include "KcOpWindowing.h"
#include "dsp/KgWindowing.h"
#include "imgui.h"


KcOpWindowing::KcOpWindowing()
    : super_("Windowing", true)
    , type_(KgWindowing::k_hamming)
    , arg_(0)
{

}


bool KcOpWindowing::onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins)
{
    assert(win_ == nullptr);
    assert(isize_() > 0);

    win_ = std::make_unique<KgWindowing>(isize_(), KgWindowing::KeType(type_), arg_);
    prepareOutput_();
    return win_ != nullptr && odata_.front() != nullptr;
}


void KcOpWindowing::onStopPipeline()
{
    win_.reset();
}


void KcOpWindowing::showProperySet()
{
    super_::showProperySet();
    ImGui::Separator();

    ImGui::BeginDisabled(working_());

    if (ImGui::BeginCombo("Type", KgWindowing::type2Str(KgWindowing::KeType(type_)))) {
        for (unsigned i = 0; i < KgWindowing::k_type_count; i++)
            if (ImGui::Selectable(KgWindowing::type2Str(KgWindowing::KeType(i)), i == type_))
                type_ = i;
        ImGui::EndCombo();
    }

    ImGui::InputFloat("Arg", &arg_);

    ImGui::EndDisabled();
}


void KcOpWindowing::op_(const kReal* in, unsigned len, kReal* out)
{
    assert(win_->idim() == len);
    win_->process(in, out);
}
