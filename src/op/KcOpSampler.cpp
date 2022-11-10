#include "KcOpSampler.h"
#include "dsp/KcSampler.h"
#include "imgui.h"
#include "imapp/KsImApp.h"
#include "imapp/KgPipeline.h"
#include "KuStrUtil.h"
#include "KtSampling.h"
#include "KtuMath.h"

namespace kPrivate
{
    bool TreePush(const char* label);

    void TreePop();
}


KcOpSampler::KcOpSampler()
    : super_("Sampler")
{

}


int KcOpSampler::spec(kIndex outPort) const
{
    KpDataSpec sp(super_::spec(outPort));
    sp.type = k_sampled;
    return sp.spec;
}


bool KcOpSampler::onNewLink(KcPortNode* from, KcPortNode* to)
{
    if (!super_::onNewLink(from, to))
        return false;

    if (to->parent().lock().get() == this) {
        auto node = from->parent().lock();
        auto prov = std::dynamic_pointer_cast<KvDataProvider>(node);
        assert(prov);

        auto d = prov->fetchData(from->index());
        if (d) {
            sampler_ = std::make_shared<KcSampler>(d);
            odata_.front() = sampler_;

            if (d->isContinued()) {
                sampCount_.resize(d->dim());
                for (unsigned i = 0; i < d->dim(); i++)
                    sampCount_[i] = sampler_->size(i);
            }
        }
    }

    return true;
}


bool KcOpSampler::onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins)
{
    return true;
}


void KcOpSampler::onStopPipeline()
{
    // sampler_ = nullptr
}


void KcOpSampler::output()
{
    if (!odata_.front()) {
        sampler_ = std::make_shared<KcSampler>(idata_.front());
        odata_.front() = sampler_;
    }
}


void KcOpSampler::showProperySet()
{
    super_::showProperySet();
    
    if (!sampler_)
        return;

    ImGui::Separator();
    if (sampler_->data()->isContinued()) {
        if (sampler_->dim() == 1) {
            if (ImGui::DragInt("Sample Count", sampCount_.data(), 1)) {
                sampleCountChanged_();
                KsImApp::singleton().pipeline().notifyOutputChanged(this, 0);
            }
        }
        else {
            if (kPrivate::TreePush("Sample Count")) {
                for (kIndex i = 0; i < sampler_->dim(); i++) {
                    std::string label("Dim");
                    label += KuStrUtil::toString(i + 1);
                    if (ImGui::DragInt(label.c_str(), sampCount_.data() + i, 1)) {
                        sampleCountChanged_();
                        KsImApp::singleton().pipeline().notifyOutputChanged(this, 0);
                    }
                }
                kPrivate::TreePop();
            }
        }
    }
    else { // 离散数据
        double minRate(1e-10), maxRate(1e10);
        if (sampler_->dim() == 1) {
            auto rate = 1. / sampler_->step(0);
            if (ImGui::DragScalar("Sample Rate", ImGuiDataType_Double,
                &rate, 1, &minRate, &maxRate) && rate > 0) {
                sampler_->reset(0, sampler_->range(0).low(), 1. / rate, sampler_->x0refs()[0]);
                KsImApp::singleton().pipeline().notifyOutputChanged(this, 0);
            }
        }
        else {
            if (kPrivate::TreePush("Sample Rate")) {
                for (kIndex i = 0; i < sampler_->dim(); i++) {
                    std::string label("Dim");
                    label += KuStrUtil::toString(i + 1);
                    auto rate = 1. / sampler_->step(i);
                    if (ImGui::DragScalar(label.c_str(), ImGuiDataType_Double,
                        &rate, 1, &minRate, &maxRate) && rate > 0) {
                        sampler_->reset(i, sampler_->range(i).low(), 1. / rate, sampler_->x0refs()[i]);
                        KsImApp::singleton().pipeline().notifyOutputChanged(this, 0);
                    }
                }
                kPrivate::TreePop();
            }
        }
    }
}


bool KcOpSampler::permitInput(int dataSpec, unsigned inPort) const
{
    KpDataSpec sp(dataSpec);
    return sp.type == k_continued || sp.type == k_array || sp.type == k_sampled;
}


bool KcOpSampler::onInputChanged(KcPortNode* outPort, unsigned inPort)
{
    auto prov = std::dynamic_pointer_cast<KvDataProvider>(outPort->parent().lock());
    assert(prov);

    auto d = prov->fetchData(outPort->index());
    if (sampler_ && sampler_->data() != d) {
        sampler_->setData(d);
        if (d->isContinued()) {
            auto cap = KtuMath<int>::product(sampCount_.data(), sampCount_.size());
            if (cap <= 1)
                cap = 1024;

            sampCount_.resize(d->dim());
            auto c = std::round(std::pow(cap, 1. / d->dim()));
            std::fill(sampCount_.begin(), sampCount_.end(), int(c));
        }
    }
        

    if (sampler_ && sampler_->data()->isContinued())
        sampleCountChanged_();

    return true;
}


void KcOpSampler::sampleCountChanged_()
{
    assert(sampler_ && sampler_->data()->isContinued());

    for (unsigned i = 0; i < sampler_->dim(); i++) {
        KtSampling<float_t> samp;
        samp.resetn(sampCount_[i], sampler_->range(i).low(), sampler_->range(i).high(),
            sampler_->x0refs()[i]);
        sampler_->reset(i, samp.low(), samp.dx(), samp.x0ref());
    }
}
