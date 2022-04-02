#include "KcSpectrumOp.h"
#include "KgSpectrum.h"
#include "KcSampled1d.h"
#include "KcSampled2d.h"
#include <assert.h>


KcSpectrumOp::KcSpectrumOp(KvDataProvider* prov)
	: KvDataOperator("spectrum", prov) 
{
	spec_ = std::make_unique<KgSpectrum>();
	syncParent();
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

KcSpectrumOp::kPropertySet KcSpectrumOp::propertySet() const
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
	for (unsigned i = 0; i < sizeof(type) / sizeof(std::pair<QString, int>); i++) {
		subProp.name = type[i].first;
		subProp.val = type[i].second;
		prop.children.push_back(subProp);
	}
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
	prop.val = spec_->countInTime();
	ps.push_back(prop);

	prop.id = kPrivate::k_freq_size;
	prop.name = tr(u8"sizeF");
	prop.desc = tr("number of samples in frequency domain");
	prop.val = spec_->countInFreq();
	ps.push_back(prop);

	return ps;
}


void KcSpectrumOp::syncParent()
{
	auto prov = dynamic_cast<KvDataProvider*>(parent());
	assert(prov);

	spec_->reset(prov->step(prov->dim() - 1), prov->length(prov->dim() - 1));
}


void KcSpectrumOp::setPropertyImpl_(int id, const QVariant& newVal)
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


kRange KcSpectrumOp::range(kIndex axis) const
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


kReal KcSpectrumOp::step(kIndex axis) const
{
	auto objp = dynamic_cast<const KvDataProvider*>(parent());
	assert(objp != nullptr);

	if (axis == objp->dim() - 1) {
		return spec_->df();
	}
	else if (axis == objp->dim()) {
		return KvData::k_nonuniform_step;
	}

	return objp->step(axis);
}


kIndex KcSpectrumOp::length(kIndex axis) const
{
	auto objp = dynamic_cast<const KvDataProvider*>(parent());
	assert(objp != nullptr);

	if (axis == objp->dim() - 1) 
		return spec_->countInFreq();

	return objp->length(axis);
}


bool KcSpectrumOp::isStream() const
{
	auto objp = dynamic_cast<const KvDataProvider*>(parent());

	if (objp->dim() == 1)
		return false; // key轴为频率轴，非时间轴，故非stream
	else
		return objp->isStream();
}


std::shared_ptr<KvData> KcSpectrumOp::processImpl_(std::shared_ptr<KvData> data)
{
	if (data->empty())
		return data;

	return data->dim() == 1 ? process1d_(data) : process2d_(data); 
}


std::shared_ptr<KvData> KcSpectrumOp::process1d_(std::shared_ptr<KvData> data)
{
	assert(data->dim() == 1);

	auto data1d = std::dynamic_pointer_cast<KcSampled1d>(data);
	assert(data1d && data->step(0) != KvData::k_nonuniform_step);

	auto res = std::make_shared<KcSampled1d>();
	spec_->porcess(*data1d, *res);

	return res;
}


std::shared_ptr<KvData> KcSpectrumOp::process2d_(std::shared_ptr<KvData> data)
{
	assert(data->dim() == 2);

	auto data2d = std::dynamic_pointer_cast<KvData2d>(data);
	assert(data2d && data->step(1) != KvData::k_nonuniform_step);
	assert(KtuMath<kReal>::almostEqualRel(data->step(1) * range(1).length(), kReal(0.5)));

	if (data2d->length(1) < 2 || data2d->range(1).empty())
		return data;

	syncParent();
	assert(spec_->countInTime() == data2d->length(1));

	auto df = spec_->df();
	if (df == KvData::k_unknown_step)
		df = 1 / data2d->range(1).length();

	auto res = std::make_shared<KcSampled2d>();

	res->resize(data->length(0), spec_->countInFreq(), data->channels());
	res->reset(0, data->range(0).low(), data->step(0));
	res->reset(1, 0, df);

	std::vector<kReal> rawData(data2d->length(1));
	for (kIndex c = 0; c < data2d->channels(); c++) {
		for (kIndex i = 0; i < data2d->length(0); i++) {
			for (kIndex j = 0; j < data2d->length(1); j++)
				rawData[j] = data2d->value(i, j, c).z;

			spec_->porcess(rawData.data());
			res->setChannel(i, rawData.data(), c);
		}
	}

	return res;
}
