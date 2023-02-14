#include "KcOpSampler.h"
#include "dsp/KcSampler.h"
#include "imguix.h"
#include "KtSampling.h"
#include "KuMath.h"
#include <string>


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


kRange KcOpSampler::range(kIndex outPort, kIndex axis) const
{
    if (odata_.front())
        return odata_.front()->range(axis);

    if (inputs_.front() == nullptr)
        return super_::range(outPort, axis); 

    auto prov = std::dynamic_pointer_cast<KvDataProvider>(inputs_.front()->parent().lock());
    if (prov && prov->isContinued(inputs_.front()->index()))
        return super_::range(outPort, axis); // 对于连续数据，range一致

    KtSampling<kReal> samp;
    samp.resetn(super_::size(outPort, axis), super_::range(outPort, axis).low(), 0);
    return { samp.low(), samp.high() };
}

kReal KcOpSampler::step(kIndex outPort, kIndex axis) const
{
    if (inputs_.front()) {
        auto prov = std::dynamic_pointer_cast<KvDataProvider>(inputs_.front()->parent().lock());
        if (!prov->isContinued(inputs_.front()->index()))
            return 1. / sampCount_[axis];
    }

    return super_::step(outPort, axis);
}


kIndex KcOpSampler::size(kIndex outPort, kIndex axis) const
{
    if (inputs_.front()) {
        auto prov = std::dynamic_pointer_cast<KvDataProvider>(inputs_.front()->parent().lock());
        if (prov->isContinued(inputs_.front()->index()))
            return sampCount_[axis];
    }

    return super_::size(outPort, axis);
}


bool KcOpSampler::onNewLink(KcPortNode* from, KcPortNode* to)
{
    if (!super_::onNewLink(from, to))
        return false;

    if (to->parent().lock().get() == this)
        syncInput_(); // idata已由super_::onNewLink更新

    return true;
}


void KcOpSampler::onDelLink(KcPortNode* from, KcPortNode* to)
{
    super_::onDelLink(from, to);

    if (to->parent().lock().get() == this) {
        odata_.front() = nullptr;
        sampCount_.clear();
    }
}


bool KcOpSampler::prepareOutput_()
{
    if (isInputUpdated())
        syncInput_();
    else
        syncOutput_();

    return false; 
}


void KcOpSampler::outputImpl_()
{
    // 此处不用做任何事情，prepareOutput_都干完了
}


void KcOpSampler::showPropertySet()
{
    super_::showPropertySet();
    
    if (!inputs_.front())
        return;

    ImGui::Separator();

    auto prov = std::dynamic_pointer_cast<KvDataProvider>(inputs_.front()->parent().lock());
    if (prov->isContinued(inputs_.front()->index())) {
        if (dim(0) == 1) {
            int c = sampCount_.front();
            if (ImGui::DragInt("Sample Count", &c, 1) && c > 0) {
                sampCount_.front() = c;
                setOutputExpired(0);
            }
        }
        else {
            if (ImGuiX::treePush("Sample Count", true)) {
                for (kIndex i = 0; i < dim(0); i++) {
                    std::string label("Dim");
                    label += std::to_string(i + 1);
                    int c = sampCount_[i];
                    if (ImGui::DragInt(label.c_str(), &c, 1) && c > 0) {
                        sampCount_[i] = c;
                        setOutputExpired(0);
                    }
                }
                ImGuiX::treePop();
            }
        }
    }
    else { // 离散数据
        float minRate(1e-10), maxRate(1e10);
        if (dim(0) == 1) {
            float rate = sampCount_[0];
            if (ImGui::DragFloat("Sample Rate", &rate, 1, minRate, maxRate, "%.2f Hz")) {
                sampCount_[0] = KuMath::clamp(rate, minRate, maxRate);
                setOutputExpired(0);
            }
        }
        else {
            if (ImGuiX::treePush("Sample Rate", true)) {
                for (kIndex i = 0; i < dim(0); i++) {
                    std::string label("Dim");
                    label += std::to_string(i + 1);
                    auto rate = sampCount_[i];
                    if (ImGui::DragFloat("Sample Rate", &rate, 1, minRate, maxRate, "%.2f Hz")) {
                        sampCount_[0] = KuMath::clamp(rate, minRate, maxRate);
                        setOutputExpired(0);
                    }
                }
                ImGuiX::treePop();
            }
        }
    }
}


bool KcOpSampler::permitInput(int dataSpec, unsigned inPort) const
{
    KpDataSpec sp(dataSpec);
    return sp.type == k_continued || sp.type == k_array || sp.type == k_sampled;
}


void KcOpSampler::syncInput_()
{
    assert(inputs_.front());

    if (idata_.front()) {
        if (odata_.front() == nullptr)
            odata_.front() = std::make_shared<KcSampler>(idata_.front());
        else
            std::dynamic_pointer_cast<KcSampler>(odata_.front())->setData(idata_.front());
    }
    else {
        odata_.front() = nullptr;
    }

    if (dim(0) != sampCount_.size()) { // 数据维度变更的重大变化，重置sampCount_

        sampCount_.resize(dim(0));

        auto prov = std::dynamic_pointer_cast<KvDataProvider>(inputs_.front()->parent().lock());
        if (prov->isContinued(inputs_.front()->index())) {
            int c = std::ceil(std::pow(1024., 1. / (dim(0))));
            std::fill(sampCount_.begin(), sampCount_.end(), c);
        }
        else {
            for (unsigned i = 0; i < dim(0); i++)
                sampCount_[i] = 1. / prov->step(inputs_.front()->index(), i);
        }
    }

    syncOutput_();
}


void KcOpSampler::syncOutput_()
{
    if (odata_.front()) {
        assert(odata_.front()->dim() == sampCount_.size());
        auto sampler = std::dynamic_pointer_cast<KcSampler>(odata_.front());
        if (sampler->data()->isContinued()) {
            for (unsigned i = 0; i < sampler->dim(); i++) {
                KtSampling<float_t> samp;
                samp.resetn(sampCount_[i], sampler->range(i).low(), sampler->range(i).high(),
                    sampler->x0refs()[i]);
                sampler->reset(i, samp.low(), samp.dx(), samp.x0ref());
            }
        }
        else {
            for (unsigned i = 0; i < sampler->dim(); i++)
                sampler->reset(i, sampler->range(i).low(), 1. / sampCount_[i], sampler->x0refs()[i]);
        }
    }
}