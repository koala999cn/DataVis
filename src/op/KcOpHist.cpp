#include "KcOpHist.h"
#include <vector>
#include "imgui.h"
#include "KcSampled1d.h"
#include "KgHist.h"


KcOpHist::KcOpHist() : super_("Hist")
{
    bins_ = 9;
    min_ = 0, max_ = 1;
}


int KcOpHist::spec(kIndex outPort) const
{
    KpDataSpec sp(super_::spec(outPort));
    sp.stream = false;
    sp.dim = 1;
    sp.type = k_sampled;
    return sp.spec;
}


kRange KcOpHist::range(kIndex outPort, kIndex axis) const
{
    if (axis == 0) 
        return { min_, max_ };

    return super_::range(outPort, axis);
}


kReal KcOpHist::step(kIndex outPort, kIndex axis) const
{
    if (axis == 0) 
        return (max_ - min_) / bins_;

    return super_::step(outPort, axis);
}


kIndex KcOpHist::size(kIndex outPort, kIndex axis) const
{
    if (axis == 0) 
        return bins_;

    return super_::step(outPort, axis);
}


bool KcOpHist::onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins)
{
    hist_ = std::make_unique<KgHist>();
    hist_->resetLinear(bins_, min_, max_);

    auto out = std::make_shared<KcSampled1d>(step(0, 0), channels(0));
    out->resize(size(0, 0));
    odata_.front() = out;

    return true;
}


void KcOpHist::onStopPipeline()
{
    hist_.reset();
    // odata_.front() = nullptr;
}


void KcOpHist::output()
{
    auto in = std::dynamic_pointer_cast<KcSampled1d>(idata_.front());
    auto out = std::dynamic_pointer_cast<KcSampled1d>(odata_.front());

    auto prov = std::dynamic_pointer_cast<KvDataProvider>(inputs_.front()->parent().lock());
    if (prov->isStream(inputs_.front()->index()))
        in->alignX0(0);

    hist_->process(*in, *out);
}


void KcOpHist::showProperySet()
{
    super_::showProperySet();
    ImGui::Separator();

    if (ImGui::DragInt("Hist Bins", &bins_, 1, 1, 999999))
        notifyChanged_();

    float low = min_, high = max_;
    if (ImGui::DragFloatRange2("Hist Range", &low, &high)) {
        min_ = low, max_ = high;
        notifyChanged_();
    }
}


bool KcOpHist::permitInput(int dataSpec, unsigned inPort) const
{
    KpDataSpec sp(dataSpec);
    return sp.type == k_sampled && sp.dim == 1; // TODO: 暂时只支持一维采样数据
}


bool KcOpHist::onInputChanged(KcPortNode* outPort, unsigned inPort)
{
    return false;
}
