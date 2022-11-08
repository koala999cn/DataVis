#include "KcOpSpectrum.h"
#include "KcSampled1d.h"
#include "KcSampled2d.h"
#include <assert.h>
#include "imgui.h"
#include "imapp/KsImApp.h"
#include "imapp/KgPipeline.h"


KcOpSpectrum::KcOpSpectrum()
	: KvDataOperator("Spectrum")
{
	
}


int KcOpSpectrum::spec(kIndex outPort) const
{
	KpDataSpec sp(super_::spec(outPort));
	sp.stream &= (sp.dim > 1);
	return sp.spec; 
}


kRange KcOpSpectrum::range(kIndex outPort, kIndex axis) const
{
	assert(inputs_.size() == 1);
	auto d = inputs_.front();
	if (d == nullptr)
		return kRange(0, 1); // 暂时无输入连接

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


kIndex KcOpSpectrum::size(kIndex outPort, kIndex axis) const
{
	assert(inputs_.size() == 1);
	auto d = inputs_.front();
	if (d == nullptr)
		return 0; // 暂时无输入连接

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(d->parent().lock());
	assert(prov);

	if (axis == prov->dim(outPort) - 1) {
		auto idim = prov->size(outPort, axis);
		return KgSpectrum::odim(idim, roundToPower2_);
	}
	
	return prov->size(outPort, axis);

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
	assert(odata_.size() == 1);
	KtSampling<double> samp;
	samp.resetn(spec_->odim(), 0, opts.sampleRate / 2, 0.5);

	if (prov->dim(node->index()) == 1) {
		auto out = std::make_shared<KcSampled1d>();
		out->reset(0, 0, samp.dx(), 0.5);
		out->resize(spec_->odim(), prov->channels(node->index()));
		odata_.front() = out;
	}
	else {
		assert(prov->dim(node->index()) == 2);
		auto out = std::make_shared<KcSampled2d>();
		out->reset(0, prov->range(node->index(), 0).low(), prov->step(node->index(), 0));
		out->reset(1, 0, samp.dx(), 0.5);
		out->resize(0, spec_->odim(), prov->channels(node->index()));
		odata_.front() = out;
	}

	return true;
}


void KcOpSpectrum::onStopPipeline()
{
	spec_.reset();
	// TODO: odata_.front() = nullptr;
}


void KcOpSpectrum::output()
{
	assert(spec_);
	assert(idata_.size() == 1 && idata_.front());
	assert(idata_.front()->isDiscreted());
	
	auto in = std::dynamic_pointer_cast<KvSampled>(idata_.front());
	assert(in);
	if (in->size(in->dim() - 1) < spec_->idim()) // TODO: 目前简单抛弃长度不足数据
		return;

	in->dim() == 1 ? output1d_() : output2d_();
}


void KcOpSpectrum::output1d_()
{
	auto in = std::dynamic_pointer_cast<KcSampled1d>(idata_.front());
	auto out = std::dynamic_pointer_cast<KcSampled1d>(odata_.front());
	assert(in && in->size(0) >= spec_->idim());
	assert(out && out->size(0) == spec_->odim());

	unsigned offset = in->size(0) - spec_->idim(); // 跳过多出的数据

	if (in->channels() == 1) {
		spec_->process(in->data() + offset, out->data());
	}
	else {
		std::vector<kReal> in_(spec_->idim());
		std::vector<kReal> out_(spec_->odim());
		for (kIndex ch = 0; ch < in->channels(); ch++) {
			for (kIndex i = 0; i < in->size(); i++)
				in_[i] = in->value(i + offset, ch);

			spec_->process(in_.data(), out_.data());
			out->setChannel(nullptr, ch, out_.data());
		}
	}
}


void KcOpSpectrum::output2d_()
{
	auto in = std::dynamic_pointer_cast<KcSampled2d>(idata_.front());
	auto out = std::dynamic_pointer_cast<KcSampled2d>(odata_.front());
	assert(in && in->size(1) >= spec_->idim());
	assert(out && out->size(1) == spec_->odim());

	out->resize(in->size(0), spec_->odim(), in->channels());
	unsigned offset = in->size(1) - spec_->idim(); // 跳过多出的数据

	if (in->channels() == 1) {
		for (unsigned r = 0; r < in->size(0); r++) 
			spec_->process(in->row(r) + offset, out->row(r));
	}
	else {
		std::vector<kReal> iData(spec_->idim()), oData(spec_->odim());

		for (kIndex ch = 0; ch < in->channels(); ch++) {
			kIndex row, col;
			for (row = 0; row < in->size(0); row++) {
				for (col = 0; col < iData.size(); col++)
					iData[col] = in->value(row, col + offset, ch);

				spec_->process(iData.data(), oData.data());
				out->setChannel(&row, ch, oData.data());
			}
		}
	}
}


void KcOpSpectrum::showProperySet()
{
	KvDataOperator::showProperySet();
	ImGui::Separator();

	bool disable = KsImApp::singleton().pipeline().running();
	ImGui::BeginDisabled(disable);

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


bool KcOpSpectrum::permitInput(int dataSpec, unsigned inPort) const
{
	KpDataSpec sp(dataSpec);
	return sp.dim <= 2 && sp.type == k_sampled; // 只接受采样数据
}

