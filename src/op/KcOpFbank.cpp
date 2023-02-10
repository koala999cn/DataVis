#include "KcOpFbank.h"
#include "KgFbank.h"
#include "KcSampled1d.h"
#include "KcSampled2d.h"
#include "imgui.h"


KcOpFbank::KcOpFbank()
	: super_("Fbank", true, true)
{
    type_ = KgFbank::k_linear;
    bins_ = 13;
}


kRange KcOpFbank::range(kIndex outPort, kIndex axis) const
{
    if (odata_[outPort])
        return odata_[outPort]->range(axis);

    auto r = super_::range(outPort, axis);

    if (axis == dim(outPort) - 1) {
        r.resetLow(KgFbank::fromHertz(KgFbank::KeType(type_), r.low()));
        r.resetHigh(KgFbank::fromHertz(KgFbank::KeType(type_), r.high()));
    }

    return r;
}


kReal KcOpFbank::step(kIndex outPort, kIndex axis) const
{
    if (axis == dim(outPort) - 1) {
        auto r = range(outPort, axis);
        KtSampling<kReal> sanpScale;
        sanpScale.resetn(bins_ + 1, r.low(), r.high(), 0); // 在目标尺度上均匀划分各bin，相邻的bin有1/2重叠
        return sanpScale.dx();
    }

    return super_::step(outPort, axis);
}


bool KcOpFbank::onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins)
{
    if (super_::onStartPipeline(ins))
        return false;

    KgFbank::KpOptions opts;
    opts.sampleRate = 1. / inputStep_(dim(0) - 1);
    opts.fftBins = inputSize_(dim(0) - 1);

    opts.type = KgFbank::KeType(type_);
    opts.lowFreq = 0;
    opts.highFreq = opts.sampleRate;
    opts.numBanks = bins_;
    opts.normalize = false; // TODO:

    fbank_ = std::make_unique<KgFbank>(opts);
    createOutputData_();

    return fbank_ != nullptr && odata_[0] != nullptr;
}


void KcOpFbank::onStopPipeline()
{
    fbank_.reset();
    super_::onStopPipeline();
}


void KcOpFbank::showPropertySet()
{
    super_::showPropertySet();
    ImGui::Separator();

    ImGui::BeginDisabled(working_());

    if (ImGui::BeginCombo("Type", KgFbank::type2Str(KgFbank::KeType(type_)))) {
        for (unsigned i = 0; i < KgFbank::k_type_count; i++)
            if (ImGui::Selectable(KgFbank::type2Str(KgFbank::KeType(i)), i == type_)) {
                type_ = i;
                setOutputExpired(0);
            }
        ImGui::EndCombo();
    }

    int bins(bins_);
    if (ImGui::DragInt("Bins", &bins, 1, 1, 4096) && bins >= 1) {
        bins_ = bins;
        setOutputExpired(0);
    }

    ImGui::EndDisabled();
}


kIndex KcOpFbank::osize_(kIndex is) const
{
    return bins_;
}


void KcOpFbank::op_(const kReal* in, unsigned len, kReal* out)
{
    assert(len == isize_());
    fbank_->process(in, out);
}


void KcOpFbank::prepareOutput_()
{

}
