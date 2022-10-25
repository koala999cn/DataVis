#include "KcOpSpectrum.h"
//#include "KgSpectrum.h"
#include "KcSampled1d.h"
#include "KcSampled2d.h"
#include <assert.h>


KcOpSpectrum::KcOpSpectrum()
	: KvDataOperator("Spectrum")
{
	
}


bool KcOpSpectrum::onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins)
{
	if (ins.empty())
		return false;

	assert(ins.size() == 1 && ins.front().first == 0);

	auto port = ins.front().second;
	auto prov = std::dynamic_pointer_cast<KvDataProvider>(port->parent().lock());
	if (prov == nullptr)
		return false; // we'll never touch here

	assert(prov->isSampled() && prov->dim() == 1);

	KgSpectrum::KpOptions opts;
	opts.sampleRate = 1.0 / prov->step(prov->dim() - 1);
	opts.frameSize = prov->size(prov->dim() - 1);
	opts.type = KgSpectrum::KeType(0);
	opts.norm = KgSpectrum::KeNormMode(0);
	opts.roundToPower2 = false;

	spec_ = std::make_unique<KgSpectrum>(opts);
	if (spec_ == nullptr)
		return false;

	// 准备output对象
	assert(outputs_.size() == 1);
	KtSampling<double> samp;
	samp.resetn(spec_->odim(), 0, opts.sampleRate / 2, 0.5);
	auto out = std::make_shared<KcSampled1d>();
	out->reset(0, 0, samp.dx(), 0.5);
	out->resize(spec_->odim(), prov->channels());
	outputs_.front() = out;

	return true;
}


void KcOpSpectrum::output()
{
	assert(spec_);
	assert(inputs_.size() == 1 && inputs_.front());
	assert(spec_->idim() == inputs_.front()->size());
	assert(inputs_.front()->isDiscreted());

	auto in = std::dynamic_pointer_cast<KcSampled1d>(inputs_.front());
	auto out = std::dynamic_pointer_cast<KcSampled1d>(outputs_.front());
	assert(in && out);

	if (in->channels() == 1) {
		spec_->process(in->data(), out->data());
	}
	else {
		std::vector<kReal> in_(in->size());
		std::vector<kReal> out_(out->size());
		for (kIndex c = 0; c < in->channels(); c++) {
			for (kIndex i = 0; i < in->size(); i++)
				in_[i] = in->value(i, c);

			spec_->process(in_.data(), out_.data());
			out->setChannel(nullptr, c, out_.data());
		}
	}
}


#if 0
KcOpSpectrum::KcOpSpectrum(KvDataProvider* prov)
	: KvDataOperator("spectrum", prov) 
{
	spec_ = std::make_unique<KgSpectrum>();
	preRender_();
}


namespace kPrivate
{
	enum KeFftPropertyId
	{
		k_spectrom_type,
		k_spectrom_floor,
		k_nyquist_freq,
		k_freq_step,
		k_time_size,
		k_freq_size,
	};
}

KcOpSpectrum::kPropertySet KcOpSpectrum::propertySet() const
{
	kPropertySet ps;

	KpProperty prop;
	KpProperty subProp;

	static const std::pair<QString, int> type[] = {
		{ "Power", KgSpectrum::k_power },
		{ "Log", KgSpectrum::k_log },
		{ "dB", KgSpectrum::k_db },
		{ "Mag", KgSpectrum::k_mag }
	};

	prop.id = kPrivate::k_spectrom_type;
	prop.name = tr("Type");
	prop.val = spec_->type();
	prop.makeEnum(type);
	ps.push_back(prop);

	prop.id = kPrivate::k_spectrom_floor;
	prop.name = tr(u8"Floor");
	prop.val = spec_->floor();
	ps.push_back(prop);

	prop.id = kPrivate::k_nyquist_freq;
	prop.name = tr(u8"Freq");
	prop.disp = tr(u8"Nyquist Frequency");
	prop.val = spec_->nyqiustFreq();
	prop.flag = k_readonly;
	ps.push_back(prop);
	
	prop.id = kPrivate::k_freq_step;
	prop.name = tr(u8"df");
	prop.disp.clear();
	prop.desc = tr("step in frequency domain");
	prop.val = spec_->df();
	ps.push_back(prop);

	prop.id = kPrivate::k_time_size;
	prop.name = tr(u8"sizeT");
	prop.desc = tr("number of samples in time domain");
	prop.val = spec_->sizeInTime();
	ps.push_back(prop);

	prop.id = kPrivate::k_freq_size;
	prop.name = tr(u8"sizeF");
	prop.desc = tr("number of samples in frequency domain");
	prop.val = spec_->sizeInFreq();
	ps.push_back(prop);

	return ps;
}


void KcOpSpectrum::preRender_()
{
	auto prov = dynamic_cast<KvDataProvider*>(parent());
	assert(prov);

	spec_->reset(prov->step(prov->dim() - 1), prov->size(prov->dim() - 1));
}


void KcOpSpectrum::setPropertyImpl_(int id, const QVariant& newVal)
{
	switch (id) {
	case kPrivate::k_spectrom_type:
		spec_->setType(newVal.toInt());
		break;

	case kPrivate::k_spectrom_floor:
		spec_->setFloor(newVal.value<kReal>());
		break;
	}
}


kRange KcOpSpectrum::range(kIndex axis) const
{
	auto objp = dynamic_cast<const KvDataProvider*>(parent());
	assert(objp != nullptr);

	// 对信号的最高维进行变换，其他低维度保持原信号尺度不变
	if (axis == objp->dim() - 1) {
		return { 0, spec_->nyqiustFreq() };
	}
	else if (axis == objp->dim()) { // 频率幅值域
		auto r = KvDataOperator::range(objp->dim());
		auto mag = KtuMath<kReal>::absMax(r.low(), r.high());
		mag = std::max(mag, spec_->floor());
		if (spec_->type() == KgSpectrum::k_log)
			mag = log(mag);
		else if (spec_->type() == KgSpectrum::k_db)
			mag = 10 * log10(mag);

		return { 0, mag };
	}
	
	return objp->range(axis);
}


kReal KcOpSpectrum::step(kIndex axis) const
{
	auto objp = dynamic_cast<const KvDataProvider*>(parent());
	assert(objp != nullptr);

	if (axis == objp->dim() - 1) {
		return spec_->df();
	}
	else if (axis == objp->dim()) {
		return KvDiscreted::k_nonuniform_step;
	}

	return objp->step(axis);
}


kIndex KcOpSpectrum::size(kIndex axis) const
{
	auto objp = dynamic_cast<const KvDataProvider*>(parent());
	assert(objp != nullptr);

	if (axis == objp->dim() - 1) 
		return spec_->sizeInFreq();

	return objp->size(axis);
}


bool KcOpSpectrum::isStream() const
{
	auto objp = dynamic_cast<const KvDataProvider*>(parent());

	if (objp->dim() == 1)
		return false; // key轴为频率轴，非时间轴，故非stream
	else
		return objp->isStream();
}


std::shared_ptr<KvData> KcOpSpectrum::processImpl_(std::shared_ptr<KvData> data)
{
	if (data->size() == 0)
		return data;

	return data->dim() == 1 ? process1d_(data) : process2d_(data); 
}


std::shared_ptr<KvData> KcOpSpectrum::process1d_(std::shared_ptr<KvData> data)
{
	assert(data->dim() == 1);

	auto res = std::make_shared<KcSampled1d>();
	spec_->porcess(*data, *res);

	return res;
}


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