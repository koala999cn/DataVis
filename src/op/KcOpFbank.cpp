#include "KcOpFbank.h"
#include "KgFbank.h"
#include "KcSampled1d.h"
#include "KcSampled2d.h"
#include "imguix.h"


KcOpFbank::KcOpFbank()
	: super_("Fbank", true, true)
{
    type_ = KgFbank::k_linear;
    bins_ = 13;
    normalize_ = false;
    low_ = 0, high_ = 0;
}


kRange KcOpFbank::range(kIndex outPort, kIndex axis) const
{
    // NB: 不使用odata_信息，因为有的地方需要进行参数匹配性检测
    // if (odata_[outPort])
    //    return odata_[outPort]->range(axis);

    if (axis == dim(outPort) - 1) {
        if (fbank_)
            return fbank_->rangeInScale();

        // 计算range
        auto low = KgFbank::fromHertz(KgFbank::KeType(type_), low_);
        auto high = KgFbank::fromHertz(KgFbank::KeType(type_), high_);
        if (low > high)
            std::swap(low, high);
        return { low, high };
    }
    else if (axis == dim(outPort)) {
        // TODO: value range
    }

    return super_::range(outPort, axis);
}


kReal KcOpFbank::step(kIndex outPort, kIndex axis) const
{
    if (axis == dim(outPort) - 1) {
        if (fbank_)
            return fbank_->stepInScale();

        KtSampling<double> samp;
        auto r = range(outPort, axis);
        samp.resetn(bins_ + 1, r.low(), r.high(), 0);
        return samp.dx();
    }

    return super_::step(outPort, axis);
}


bool KcOpFbank::onNewLink(KcPortNode* from, KcPortNode* to)
{
    if (!super_::onNewLink(from, to))
        return false;

    auto r = inputRange_(dim(0) - 1);
    low_ = r.low(), high_ = r.high();

    return true;
}


bool KcOpFbank::onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins)
{
    if (!super_::onStartPipeline(ins))
        return false;

    assert(fbank_ == nullptr);
    prepareOutput_(); // 创建fbank_对象
    createOutputData_();

    return fbank_  && odata_.front();
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

    if (ImGui::BeginCombo("Type", KgFbank::type2Str(KgFbank::KeType(type_)))) {
        for (unsigned i = 0; i < KgFbank::k_type_count; i++)
            if (ImGui::Selectable(KgFbank::type2Str(KgFbank::KeType(i)), i == type_)) {
                type_ = i;
                setOutputExpired(0);
            }
        ImGui::EndCombo();
    }

    float low(low_), high(high_);
    if (ImGui::DragFloatRange2("Range", &low, &high, 1, 0, 0, "%.1f Hz")
        && low >= 0 && high >= low) {
        low_ = low, high_ = high;
        setOutputExpired(0);
    }

    int bins(bins_);
    if (ImGui::DragInt("Bins", &bins, 1, 1, 4096) && bins >= 1) {
        bins_ = bins;
        setOutputExpired(0);
    }

    if (ImGui::Checkbox("Normalize", &normalize_) && fbank_)
        fbank_->setNormalize(normalize_); // 无须调用setOutputExpired

    if (fbank_) {
        ImGui::Separator();

        if (ImGuiX::treePush("Bins Info", false)) {
            std::vector<double> fcInScale(fbank_->options().numBanks);
            for (unsigned i = 0; i < fbank_->options().numBanks; i++)
                fcInScale[i] = KgFbank::fromHertz(KgFbank::KeType(type_), fbank_->fc(i));

            auto showBins = [this, &fcInScale](unsigned i, unsigned j) {
                if (j == 0) {
                    ImGui::Text("%d", i + 1);
                }
                else {
                    auto f = fcInScale[i];
                    if (j == 1) 
                        f -= fbank_->stepInScale();
                    else if (j == 3)
                        f += fbank_->stepInScale();
                    if (f < 0) f = 0; // 假定各尺度的频率值都大于0，此处检测因计算误差可能出现的负值，避免toHertz返回nan

                    if (type_ == KgFbank::k_linear)
                        ImGui::Text("%.1fHz", f);
                    else
                        ImGui::Text("%.1fHz/%.1f%s", KgFbank::toHertz(KgFbank::KeType(type_), f),
                            f, KgFbank::type2Str(KgFbank::KeType(type_)));
                }
            };

            ImGuiX::showLargeTable(fbank_->options().numBanks, 4, showBins, 1, 1, {
                "bin", "left-freq", "center-freq", "right-freq"
                });

            ImGuiX::treePop();
        }
    }
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


bool KcOpFbank::prepareOutput_()
{
    if (fbank_ == nullptr 
        || fbank_->idim() != isize_() // fftBins发生变化
        || isOutputExpired()) {
        KgFbank::KpOptions opts;
        opts.sampleRate = inputRange_(dim(0) - 1).high();
        opts.fftBins = isize_();

        opts.type = KgFbank::KeType(type_);
        opts.lowFreq = low_;
        opts.highFreq = high_;
        opts.numBanks = bins_;
        opts.normalize = normalize_;

        fbank_ = std::make_unique<KgFbank>(opts);

        return fbank_ != nullptr;
    }

    return false;
}
