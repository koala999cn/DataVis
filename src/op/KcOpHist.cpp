#include "KcOpHist.h"
#include <vector>
#include "imgui.h"
#include "KcSampled1d.h"
#include "KgHist.h"
#include "KuDataUtil.h"


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
    if (axis == 0) {
        KtSampling<kReal> samp;
        samp.resetn(bins_, min_, max_, 0.5);
        return samp.dx();
    }

    return super_::step(outPort, axis);
}


kIndex KcOpHist::size(kIndex outPort, kIndex axis) const
{
    if (axis == 0) 
        return bins_;

    return super_::step(outPort, axis);
}


bool KcOpHist::onNewLink(KcPortNode* from, KcPortNode* to)
{
    if (!super_::onNewLink(from, to))
        return false;

    auto r = inputRange_(0); // 输入数据的x轴范围
    min_ = r.low(), max_ = r.high();
    return true;
}


bool KcOpHist::onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins)
{
    if (!super_::onStartPipeline(ins))
        return false;

    hist_ = std::make_unique<KgHist>();
    hist_->resetLinear(bins_, min_, max_);

    auto out = std::make_shared<KcSampled1d>(step(0, 0), channels(0));
    out->resize(bins_);
    odata_.front() = out;

    return true;
}


void KcOpHist::onStopPipeline()
{
    hist_.reset();
    super_::onStopPipeline();
}


bool KcOpHist::prepareOutput_()
{
    if (isOutputExpired()) {
        auto out = std::dynamic_pointer_cast<KcSampled1d>(odata_.front());
        out->resize(bins_, channels(0)); 
        out->reset(0, min_, step(0, 0), 0.5);

        hist_->resetLinear(bins_, min_, max_);
        return true;
    }

    return false;
}


void KcOpHist::outputImpl_()
{
    auto in = std::dynamic_pointer_cast<KvSampled>(idata_.front());
    auto out = std::dynamic_pointer_cast<KcSampled1d>(odata_.front());

    KpDataSpec ds(inputSpec_());
    if (ds.stream) // 此处不能使用isStream，因为KcOpHist重置了stream标记
        in->reset(0, 0, in->step(0)); // 流式数据的x轴低值可能随时间递增，此时强制归零

    hist_->process(*in, out->data());
}


void KcOpHist::showPropertySet()
{
    super_::showPropertySet();
    ImGui::Separator();

    int bins(bins_);
    if (ImGui::DragInt("Hist Bins", &bins, 1, 1, 999999) && bins > 0) {
        bins_ = bins;
        setOutputExpired(0);
    }

    float low = min_, high = max_;
    if (ImGui::DragFloatRange2("Hist Range", &low, &high) && high >= low) {
        min_ = low, max_ = high;
        setOutputExpired(0);
    }
}


bool KcOpHist::permitInput(int dataSpec, unsigned inPort) const
{
    KpDataSpec sp(dataSpec);
    return (sp.type == k_sampled || sp.type == k_array) && sp.dim == 1; // TODO: 暂时只支持一维采样数据
}
