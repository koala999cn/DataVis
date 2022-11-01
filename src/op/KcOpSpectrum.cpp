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
	sp.stream &= sp.dim > 1; // 输入若为高维数据，则该节点streaming
	return sp.spec; 
}


kRange KcOpSpectrum::range(kIndex outPort, kIndex axis) const
{
	assert(inputs_.size() == 1);
	auto d = inputs_.front();
	if (d == nullptr)
		return kRange(0, 1); // 暂时无输入连接

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(d->parent().lock());
	if (prov == nullptr)
		return kRange(0, 1); // 输入失效

	// 对信号的最高维进行变换，其他低维度保持原信号尺度不变
	if (axis == dim(outPort) - 1) 
		return spec_ ? kRange{ 0, spec_->options().sampleRate / 2 } :
		               kRange{ 0, 0.5 / prov->step(d->index(), axis) };

	auto r = prov->range(d->index(), axis);
	if (axis == dim(outPort) && spec_) // 频率幅值域
		return spec_->orange({ r.low(), r.high() }); 
	
	return r;
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
	auto out = std::make_shared<KcSampled1d>();
	out->reset(0, 0, samp.dx(), 0.5);
	out->resize(spec_->odim(), prov->channels(node->index()));
	odata_.front() = out;

	return true;
}


void KcOpSpectrum::output()
{
	assert(spec_);
	assert(idata_.size() == 1 && idata_.front());
	assert(idata_.front()->isDiscreted());
	// assert(spec_->idim() == inputs_.front()->size()); 该断言不成立
	
	if (idata_.front()->size() < spec_->idim()) // TODO: 目前简单抛弃长度不足数据
		return; 

	auto in = std::dynamic_pointer_cast<KcSampled1d>(idata_.front());
	auto out = std::dynamic_pointer_cast<KcSampled1d>(odata_.front());
	assert(in && out);

	unsigned offset = idata_.front()->size() - spec_->idim();

	if (in->channels() == 1) {
		spec_->process(in->data() + offset, out->data());
	}
	else {
		std::vector<kReal> in_(spec_->idim());
		std::vector<kReal> out_(spec_->odim());
		for (kIndex c = 0; c < in->channels(); c++) {
			for (kIndex i = 0; i < in->size(); i++)
				in_[i] = in->value(i + offset, c);

			spec_->process(in_.data(), out_.data());
			out->setChannel(nullptr, c, out_.data());
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
			if (ImGui::Selectable(KgSpectrum::type2Str(i), i == specType_))
				specType_ = i;
		ImGui::EndCombo();
	}

	auto curNorm = KgSpectrum::norm2Str(normMode_);
	if (ImGui::BeginCombo("Normalization", curNorm)) {
		for (unsigned i = 0; i < KgSpectrum::normModeCount(); i++)
			if (ImGui::Selectable(KgSpectrum::norm2Str(i), i == normMode_))
				normMode_ = i;
		ImGui::EndCombo();
	}

	ImGui::Checkbox("Round to Power of 2", &roundToPower2_);

	ImGui::EndDisabled();
}


bool KcOpSpectrum::permitInput(int dataSpec, unsigned inPort) const
{
	KpDataSpec sp(dataSpec);
	return sp.dim == 1 && sp.type == k_sampled; // 只接受采样数据
}

#if 0

std::shared_ptr<KvData> KcOpSpectrum::process2d_(std::shared_ptr<KvData> data)
{
	assert(data->dim() == 2);
	auto samp = std::dynamic_pointer_cast<KvSampled>(data);
	assert(samp);
	assert(KtuMath<kReal>::almostEqualRel(samp->step(1) * range(1).length(), kReal(0.5)));

	if (samp->size(1) < 2 || samp->range(1).empty())
		return data;

	assert(spec_->sizeInTime() == samp->size(1));

	auto df = spec_->df();
	assert(df > 0);

	auto res = std::make_shared<KcSampled2d>();

	res->resize(samp->size(0), spec_->sizeInFreq(), samp->channels());
	res->reset(0, samp->range(0).low(), samp->step(0));
	res->reset(1, 0, df);

	std::vector<kReal> rawData(samp->size(1));
	for (kIndex c = 0; c < samp->channels(); c++) {
		kIndex idx[2];
		for (idx[0] = 0; idx[0] < samp->size(0); idx[0]++) {
			for (idx[1] = 0; idx[1] < samp->size(1); idx[1]++)
				rawData[idx[1]] = samp->value(idx, c);

			spec_->porcess(rawData.data());
			res->setChannel(idx, c, rawData.data());
		}
	}

	return res;
}
#endif