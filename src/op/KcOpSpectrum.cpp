#include "KcOpSpectrum.h"
#include "KcSampled1d.h"
#include "KcSampled2d.h"
#include <assert.h>
#include "imgui.h"
#include "imapp/KsImApp.h"
#include "imapp/KgPipeline.h"


KcOpSpectrum::KcOpSpectrum()
	: super_("Spectrum", true, true)
{
	
}


int KcOpSpectrum::spec(kIndex outPort) const
{
	KpDataSpec ds = super_::spec(outPort);
	if (ds.stream && ds.dim == 1) {
		assert(ds.dynamic);
		ds.stream = false;
	}

	return ds.spec;
}


kRange KcOpSpectrum::range(kIndex outPort, kIndex axis) const
{
	// 对信号的最高维进行变换，其他低维度保持原信号尺度不变
	if (axis == dim(outPort) - 1) {
		auto dx = inputStep_(dim(0) - 1);
		return { 0, dx == 0 ? dx : 0.5 / dx }; // 奈奎斯特频率
	}

	auto r = inputRange_(axis);
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

	prepareOutput_(); // 创建spec_
	createOutputData_(); // 初始化odata

	return true;
}


void KcOpSpectrum::onStopPipeline()
{
	spec_.reset();
	super_::onStopPipeline();
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


bool KcOpSpectrum::prepareOutput_()
{
	auto isize = inputSize_(dim(0) - 1);
	if (isOutputExpired() || !spec_ || isize != spec_->idim()) {
		KgSpectrum::KpOptions opts;
		opts.sampleRate = 1.0 / inputStep_(dim(0) - 1);
		opts.frameSize = isize;
		opts.type = KgSpectrum::KeType(specType_);
		opts.norm = KgSpectrum::KeNormMode(normMode_);
		opts.roundToPower2 = roundToPower2_;

		if (spec_)
			spec_->reset(opts);
		else
			spec_ = std::make_unique<KgSpectrum>(opts);
		return true;
	}

	return false;
}


void KcOpSpectrum::op_(const kReal* in, unsigned len, unsigned ch, kReal* out)
{
	assert(spec_ && spec_->idim() == len);
	assert(spec_->options().sampleRate == 1. / inputStep_(dim(0) - 1));
	spec_->process(in, out);
}


void KcOpSpectrum::showPropertySet()
{
	KvDataOperator::showPropertySet();
	ImGui::Separator();

	auto curType = KgSpectrum::type2Str(specType_);
	if (ImGui::BeginCombo("Spectrum Type", curType)) {
		for (unsigned i = 0; i < KgSpectrum::typeCount(); i++)
			if (ImGui::Selectable(KgSpectrum::type2Str(i), i == specType_)) {
				specType_ = i;
				setOutputExpired(0);
			}
		ImGui::EndCombo();
	}

	auto curNorm = KgSpectrum::norm2Str(normMode_);
	if (ImGui::BeginCombo("Normalization", curNorm)) {
		for (unsigned i = 0; i < KgSpectrum::normModeCount(); i++)
			if (ImGui::Selectable(KgSpectrum::norm2Str(i), i == normMode_)) {
				normMode_ = i;
				setOutputExpired(0);
			}
		ImGui::EndCombo();
	}

	if (ImGui::Checkbox("Round to Power of 2", &roundToPower2_))
		setOutputExpired(0);
}
