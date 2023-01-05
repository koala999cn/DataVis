#include "KcOpSampler.h"
#include "dsp/KcSampler.h"
#include "imguix.h"
#include "KuStrUtil.h"
#include "KtSampling.h"
#include "KtuMath.h"


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
    if (sampler_ && axis < dim(0))
        return sampler_->range(axis);

    if (!inputs_.empty() && inputs_.front())
        return super_::isContinued(outPort) || axis == dim(0) ? super_::range(outPort, axis)
            : kRange(super_::range(outPort, axis).low(),
                super_::range(outPort, axis).low() + super_::size(outPort, axis) / kReal(sampCount_[axis]));

    return kRange(0, 0);
}


kReal KcOpSampler::step(kIndex outPort, kIndex axis) const
{
    if (sampler_)
        return sampler_->step(axis);

    if (!inputs_.empty() && inputs_.front())
        return super_::isContinued(outPort) ?
            super_::range(outPort, axis).length() / sampCount_[axis] : 1. / sampCount_[axis];

    return 0;
}


kIndex KcOpSampler::size(kIndex outPort, kIndex axis) const
{
    if (sampler_)
        return sampler_->size(axis);

    if (!inputs_.empty() && inputs_.front())
        return super_::isContinued(outPort) ? sampCount_[axis] : super_::size(outPort, axis);

    return 0;
}


bool KcOpSampler::onNewLink(KcPortNode* from, KcPortNode* to)
{
    if (!super_::onNewLink(from, to))
        return false;

    if (to->parent().lock().get() == this) {
        auto node = from->parent().lock();
        auto prov = std::dynamic_pointer_cast<KvDataProvider>(node);
        assert(prov);

        auto oport = from->index();
        sampCount_.resize(prov->dim(oport));

        if (prov->isContinued(oport)) {
            for (unsigned i = 0; i < prov->dim(oport); i++)
                sampCount_[i] = prov->size(oport, i);
        }
        else {
            for (unsigned i = 0; i < prov->dim(oport); i++)
                sampCount_[i] = 1. / prov->step(oport, i);
        }

        auto d = prov->fetchData(from->index());
        if (d) {
            sampler_ = std::make_shared<KcSampler>(d);
            odata_.front() = sampler_;
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
        sampleCountChanged_();
    }

    sampler_->setData(idata_.front());
}


void KcOpSampler::showProperySet()
{
    super_::showProperySet();
    
    if (!inputs_.front())
        return;

    ImGui::Separator();
    ImGui::BeginDisabled(working_());

    auto oport = inputs_.front()->index();
    if (super_::isContinued(oport)) {
        if (dim(0) == 1) {
            int c = sampCount_.front();
            if (ImGui::DragInt("Sample Count", &c, 1) && c > 0) {
                sampCount_.front() = c;
                sampleCountChanged_();
            }
        }
        else {
            if (ImGuiX::treePush("Sample Count", true)) {
                for (kIndex i = 0; i < dim(0); i++) {
                    std::string label("Dim");
                    label += KuStrUtil::toString(i + 1);
                    int c = sampCount_[i];
                    if (ImGui::DragInt(label.c_str(), &c, 1) && c > 0) {
                        sampCount_[i] = c;
                        sampleCountChanged_();
                    }
                }
                ImGuiX::treePop();
            }
        }
    }
    else { // 离散数据
        double minRate(1e-10), maxRate(1e10);
        if (dim(0) == 1) {
            auto rate = sampler_ ? 1. / sampler_->step(0) : sampCount_[0];
            if (ImGui::DragScalar("Sample Rate", ImGuiDataType_Double,
                &rate, 1, &minRate, &maxRate) && rate > 0) {
                sampCount_[0] = rate;
                sampleCountChanged_();
            }
        }
        else {
            if (ImGuiX::treePush("Sample Rate", true)) {
                for (kIndex i = 0; i < dim(0); i++) {
                    std::string label("Dim");
                    label += KuStrUtil::toString(i + 1);
                    auto rate = sampler_ ? 1. / sampler_->step(i) : sampCount_[i];
                    if (ImGui::DragScalar(label.c_str(), ImGuiDataType_Double,
                        &rate, 1, &minRate, &maxRate) && rate > 0) {
                        sampCount_[i] = rate;
                        sampleCountChanged_();
                    }
                }
                ImGuiX::treePop();
            }
        }
    }

    ImGui::EndDisabled();
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

    auto cap = KtuMath<int>::product(sampCount_.data(), sampCount_.size());
    if (cap <= 1)
        cap = 1024;

    sampCount_.resize(prov->dim(outPort->index()));
    auto c = std::round(std::pow(cap, 1. / prov->dim(outPort->index())));
    std::fill(sampCount_.begin(), sampCount_.end(), int(c));
    sampleCountChanged_();

    auto d = prov->fetchData(outPort->index());
    if (sampler_ && sampler_->data() != d) {
        sampler_->setData(d);
    }

    return true;
}


void KcOpSampler::sampleCountChanged_()
{
    if (sampler_ && sampler_->data()) {
        assert(sampler_->dim() == sampCount_.size());

        if (sampler_->data()->isContinued()) {
            for (unsigned i = 0; i < sampler_->dim(); i++) {
                KtSampling<float_t> samp;
                samp.resetn(sampCount_[i], sampler_->range(i).low(), sampler_->range(i).high(),
                    sampler_->x0refs()[i]);
                sampler_->reset(i, samp.low(), samp.dx(), samp.x0ref());
            }
        }
        else {
            for (unsigned i = 0; i < sampler_->dim(); i++) 
                sampler_->reset(i, sampler_->range(i).low(), 1. / sampCount_[i], sampler_->x0refs()[i]);
        }
    }

    notifyChanged_();
}
