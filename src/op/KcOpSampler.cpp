#include "KcOpSampler.h"
#include "dsp/KcSampler.h"


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
}


bool KcOpSampler::permitInput(int dataSpec, unsigned inPort) const
{
    KpDataSpec sp(dataSpec);
    return sp.type == k_continued || sp.type == k_array || sp.type == k_sampled;
}


bool KcOpSampler::onInputChanged(KcPortNode* outPort, unsigned inPort)
{
    return false;
}

