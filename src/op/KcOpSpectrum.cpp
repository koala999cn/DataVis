#include "KcOpSpectrum.h"
#include "KcSampled1d.h"
#include "KcSampled2d.h"
#include <assert.h>
#include "imgui.h"
#include "imapp/KsImApp.h"
#include "imapp/KgPipeline.h"


KcOpSpectrum::KcOpSpectrum()
	: super_("Spectrum", true)
{
	
}


kRange KcOpSpectrum::range(kIndex outPort, kIndex axis) const
{
	assert(inputs_.size() == 1);
	auto d = inputs_.front();
	if (d == nullptr)
		return kRange(0, 0); // 暂时无输入连接

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(d->parent().lock());
	assert(prov);

	// 对信号的最高维进行变换，其他低维度保持原信号尺度不变
	if (axis == dim(outPort) - 1) 
		return spec_ ? kRange{ 0, spec_->options().sampleRate / 2 } :
		               kRange{ 0, 0.5 / prov->step(d->index(), axis) };

	auto r = prov->range(d->index(), axis);
	if (axis == dim(outPort) && spec_) // 频率幅值域
		return spec_->orange({ r.low(), r.high() }); 
	
	return r;
}


kReal KcOpSpectrum::step(kIndex outPort, kIndex axis) const
{
	assert(inputs_.size() == 1);
	auto d = inputs_.front();
	if (d == nullptr)
		return 0; // 暂时无输入连接

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(d->parent().lock());
	assert(prov);

	if (axis == prov->dim(outPort) - 1) {
		auto idim = prov->size(outPort, axis);
		auto istep = prov->step(outPort, axis);
		KtSampling<double> samp;
		samp.resetn(KgSpectrum::odim(idim, roundToPower2_), 0, 0.5 / istep, 0.5);
		return samp.dx();
	}
	else if (axis == prov->dim(outPort)) {
		return KvDiscreted::k_nonuniform_step;
	}

	return prov->step(outPort, axis);
}


bool KcOpSpectrum::onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins)
{
	if (!super_::onStartPipeline(ins))
		return false;

	auto node = ins.front().second;
	auto prov = std::dynamic_pointer_cast<KvDataProvider>(node->parent().lock());

	KgSpectrum::KpOptions opts;
	opts.sampleRate = 1.0 / prov->step(node->index(), prov->dim(node->index()) - 1);
	opts.frameSize = prov->size(node->index(), prov->dim(node->index()) - 1);
	opts.type = KgSpectrum::KeType(specType_);
	opts.norm = KgSpectrum::KeNormMode(normMode_);
	opts.roundToPower2 = roundToPower2_;

	spec_ = std::make_unique<KgSpectrum>(opts);
	if (spec_ == nullptr)
		return false;

	// 准备output对象
	super_::prepareOutput_();

	return true;
}


void KcOpSpectrum::onStopPipeline()
{
	spec_.reset();
	// TODO: odata_.front() = nullptr;
}


kIndex KcOpSpectrum::isize_() const
{
	if (inputs_.front() == nullptr)
		return 0;

	return spec_ ? spec_->idim() : inputSize_(dim(0) - 1);
}


kIndex KcOpSpectrum::osize_(kIndex is) const
{
	return spec_ ? spec_->odim() : KgSpectrum::odim(is, roundToPower2_);
}


void KcOpSpectrum::op_(const kReal* in, unsigned len, kReal* out)
{
	assert(spec_);
	assert(len == spec_->idim());
	spec_->process(in, out);
}


void KcOpSpectrum::showProperySet()
{
	KvDataOperator::showProperySet();
	ImGui::Separator();

	ImGui::BeginDisabled(working_());

	auto curType = KgSpectrum::type2Str(specType_);
	if (ImGui::BeginCombo("Spectrum Type", curType)) {
		for (unsigned i = 0; i < KgSpectrum::typeCount(); i++)
			if (ImGui::Selectable(KgSpectrum::type2Str(i), i == specType_)) {
				specType_ = i;
				notifyChanged_();
			}
		ImGui::EndCombo();
	}

	auto curNorm = KgSpectrum::norm2Str(normMode_);
	if (ImGui::BeginCombo("Normalization", curNorm)) {
		for (unsigned i = 0; i < KgSpectrum::normModeCount(); i++)
			if (ImGui::Selectable(KgSpectrum::norm2Str(i), i == normMode_)) {
				normMode_ = i;
				notifyChanged_();
			}
		ImGui::EndCombo();
	}

	if (ImGui::Checkbox("Round to Power of 2", &roundToPower2_))
		notifyChanged_();

	ImGui::EndDisabled();
}
