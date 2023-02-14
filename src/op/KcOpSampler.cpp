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

        if (prov->isContinued(oport)) { // 对于连续数据，初始化sampCount_
            auto cap = KuMath::product(sampCount_.data(), sampCount_.size());
            if (cap <= 1) cap = 1024;
            auto c = std::round(std::pow(cap, 1. / prov->dim(oport)));
            std::fill(sampCount_.begin(), sampCount_.end(), int(c));

        }
        else {// 对于离散数据，置sampCount_等于数据size
            for (unsigned i = 0; i < prov->dim(oport); i++)
                sampCount_[i] = prov->size(oport, i);
        }

        auto d = prov->fetchData(from->index());
        if (d) {
            sampler_ = std::make_shared<KcSampler>(d);
            odata_.front() = sampler_;
        }
    }

    return true;
}


void KcOpSampler::onDelLink(KcPortNode* from, KcPortNode* to)
{
    super_::onDelLink(from, to);
    sampler_ = nullptr;
}


bool KcOpSampler::prepareOutput_()
{
    if (isInputUpdated()) {
        if (idata_.front() == nullptr) {
            odata_.front() = sampler_ = nullptr;
        }
        else {
            if (sampler_ == nullptr) {
                sampler_ = std::make_shared<KcSampler>(idata_.front());
                odata_.front() = sampler_;
            }
            sampCount_.resize(idata_.front()->dim(), 1);
            sampler_->setData(idata_.front());
        }
    }

    if (sampler_ && (isOutputExpired() || isInputUpdated())) { // NB: 数据更新后也须同步于sampCount_
        assert(sampler_->data() && sampler_->dim() == sampCount_.size());

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

    return false; // 不需要配置odata参数
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
        double minRate(1e-10), maxRate(1e10);
        if (dim(0) == 1) {
            auto rate = sampler_ ? 1. / sampler_->step(0) : sampCount_[0];
            if (ImGui::DragScalar("Sample Rate", ImGuiDataType_Double,
                &rate, 1, &minRate, &maxRate) && rate > 0) {
                sampCount_[0] = rate;
                setOutputExpired(0);
            }
        }
        else {
            if (ImGuiX::treePush("Sample Rate", true)) {
                for (kIndex i = 0; i < dim(0); i++) {
                    std::string label("Dim");
                    label += std::to_string(i + 1);
                    auto rate = sampler_ ? 1. / sampler_->step(i) : sampCount_[i];
                    if (ImGui::DragScalar(label.c_str(), ImGuiDataType_Double,
                        &rate, 1, &minRate, &maxRate) && rate > 0) {
                        sampCount_[i] = rate;
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


bool KcOpSampler::onInputChanged(KcPortNode* outPort, unsigned inPort)
{
/*  auto prov = std::dynamic_pointer_cast<KvDataProvider>(outPort->parent().lock());
    assert(prov);

    auto cap = KuMath::product(sampCount_.data(), sampCount_.size());
    if (cap <= 1)
        cap = 1024;

    sampCount_.resize(prov->dim(outPort->index()));
    auto c = std::round(std::pow(cap, 1. / prov->dim(outPort->index())));
    std::fill(sampCount_.begin(), sampCount_.end(), int(c));
    sampleCountChanged_();

    auto d = prov->fetchData(outPort->index());
    if (sampler_ && sampler_->data() != d) {
        sampler_->setData(d);
    }*/

    return true;
}


void KcOpSampler::sampleCountChanged_()
{
    /*
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

    setOutputExpired(0);*/
}
