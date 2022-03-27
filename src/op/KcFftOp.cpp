#include "KcFftOp.h"
#include "KgRdft.h"
#include "KvData1d.h"
#include "KvData2d.h"
#include <assert.h>
#include "KcSampled1d.h"
#include "KcSampled2d.h"
#include "KtuMath.h"


KcFftOp::KcFftOp(KvDataProvider* prov)
	: KvDataOperator("fft", prov) 
	, df_(KvData::k_unknown_step) 
	, nyquistFreq_(0)
{
	syncParent();
}


namespace kPrivate
{
	enum KeFftPropertyId
	{
		k_nyquist_freq,
		k_freq_step,
		k_time_size,
		k_freq_size,
	};
}

KcFftOp::kPropertySet KcFftOp::propertySet() const
{
	kPropertySet ps;

	KpProperty prop;
	prop.id = kPrivate::k_nyquist_freq;
	prop.name = tr(u8"Freq");
	prop.disp = tr(u8"Nyquist Frequency");
	prop.val = nyquistFreq_;
	prop.flag = k_readonly;
	ps.push_back(prop);
	
	prop.id = kPrivate::k_freq_step;
	prop.name = tr(u8"df");
	prop.disp.clear();
	prop.desc = tr("step in frequency domain");
	prop.val = df_;
	ps.push_back(prop);

	prop.id = kPrivate::k_time_size;
	prop.name = tr(u8"sizeT");
	prop.desc = tr("number of samples in time domain");
	prop.val = rdft_->sizeT();
	ps.push_back(prop);

	prop.id = kPrivate::k_freq_size;
	prop.name = tr(u8"sizeF");
	prop.desc = tr("number of samples in frequency domain");
	prop.val = rdft_->sizeF();
	ps.push_back(prop);

	return ps;
}


void KcFftOp::syncParent()
{
	auto prov = dynamic_cast<KvDataProvider*>(parent());
	assert(prov);

	if (rdft_ == nullptr || rdft_->sizeT() != prov->length(prov->dim() - 1)) {
		rdft_ = std::make_unique<KgRdft>(prov->length(prov->dim() - 1), false, true);

		if (prov->step(prov->dim() - 1) != KvData::k_unknown_step)
			nyquistFreq_ = 1 / prov->step(prov->dim() - 1) / 2;
		else
			nyquistFreq_ = 8000; // 未知情况下，假定8khz

		df_ = nyquistFreq_ / rdft_->sizeF();
		// 此处没有采用df = 1/T的计算公式，主要原因有2：
		// 一是此处获取的T值不准确，大多数时候比真实的T值大1个dt；
		// 二是由此根据df推导出的频域range比nyquist频率大1-2个df。
		// 例如：当采样点为偶数N，频率采样点为N/2+1，此时计算的F' = df * (N/2+1) = F + 2*df 
	}
}


void KcFftOp::setPropertyImpl_(int id, const QVariant& newVal)
{
	
}


kRange KcFftOp::range(kIndex axis) const
{
	auto objp = dynamic_cast<const KvDataProvider*>(parent());
	assert(objp != nullptr);

	// 对信号的最高维进行变换，其他低维度保持原信号尺度不变
	if (axis == objp->dim() - 1) {
		return { 0, nyquistFreq_ };
	}
	else if (axis == objp->dim()) { // 频率幅值域
		auto r = KvDataOperator::range(objp->dim());
		return { 0, KtuMath<kReal>::absMax(r.low(), r.high()) };
	}
	
	return objp->range(axis);
}


kReal KcFftOp::step(kIndex axis) const
{
	auto objp = dynamic_cast<const KvDataProvider*>(parent());
	assert(objp != nullptr);

	if (axis == objp->dim() - 1) {
		return df_;
	}
	else if (axis == objp->dim()) {
		return KvData::k_nonuniform_step;
	}

	return objp->step(axis);
}


kIndex KcFftOp::length(kIndex axis) const
{
	auto objp = dynamic_cast<const KvDataProvider*>(parent());
	assert(objp != nullptr);

	if (axis == objp->dim() - 1) 
		return rdft_->sizeF();

	return objp->length(axis);
}


std::shared_ptr<KvData> KcFftOp::processImpl_(std::shared_ptr<KvData> data)
{
	if (data->empty())
		return data;

	return data->dim() == 1 ? process1d_(data) : process2d_(data); 
}


std::shared_ptr<KvData> KcFftOp::process1d_(std::shared_ptr<KvData> data)
{
	assert(data->dim() == 1);

	auto data1d = std::dynamic_pointer_cast<KvData1d>(data);
	assert(data1d && data->step(0) != KvData::k_nonuniform_step);

	if (data1d->count() < 2 || data1d->range(0).empty())
		return data;

	assert(rdft_->sizeT() == data1d->count());
	//if (rdft_ == nullptr || rdft_->sizeT() != data1d->count())
	//	rdft_ = std::make_unique<KgRdft>(data1d->count(), false, true);

	auto df = df_;
	if (df == KvData::k_unknown_step)
		df = 1 / data1d->range(0).length();

	auto res = std::make_shared<KcSampled1d>();

	res->reset(df, data1d->channels(), rdft_->sizeF());
	assert(res->count() == rdft_->sizeF());

	// TODO: 优化单通道的情况
	std::vector<kReal> rawData(data1d->count());
	for (kIndex c = 0; c < data1d->channels(); c++) {
		for (kIndex i = 0; i < data1d->count(); i++)
			rawData[i] = data1d->value(i, c).y;

		rdft_->forward(rawData.data());
		rdft_->powerSpectrum(rawData.data());
		res->setChannel(rawData.data(), c);
	}

	return res;
}


std::shared_ptr<KvData> KcFftOp::process2d_(std::shared_ptr<KvData> data)
{
	assert(data->dim() == 2);

	auto data2d = std::dynamic_pointer_cast<KvData2d>(data);
	assert(data2d && data->step(1) != KvData::k_nonuniform_step);
	assert(KtuMath<kReal>::almostEqualRel(data->step(1) * range(1).length(), kReal(0.5)));

	if (data2d->length(1) < 2 || data2d->range(1).empty())
		return data;

	syncParent();
	assert(rdft_->sizeT() == data2d->length(1));
	//if (rdft_ == nullptr || rdft_->sizeT() != data2d->length(1))
	//	rdft_ = std::make_unique<KgRdft>(data2d->length(1), false, true);

	auto df = df_;
	if (df == KvData::k_unknown_step)
		df = 1 / data2d->range(1).length();

	auto res = std::make_shared<KcSampled2d>();

	res->resize(data->length(0), rdft_->sizeF(), data->channels());
	res->reset(0, data->range(0).low(), data->step(0));
	res->reset(1, 0, df);

	std::vector<kReal> rawData(data2d->length(1));
	for (kIndex c = 0; c < data2d->channels(); c++) {
		for (kIndex i = 0; i < data2d->length(0); i++) {
			for (kIndex j = 0; j < data2d->length(1); j++)
				rawData[j] = data2d->value(i, j, c).z;

			rdft_->forward(rawData.data());
			rdft_->powerSpectrum(rawData.data());
			res->setChannel(i, rawData.data(), c);
		}
	}

	return res;
}