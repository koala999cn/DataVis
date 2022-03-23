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
{

}


KcFftOp::kPropertySet KcFftOp::propertySet() const
{
	kPropertySet ps;

	KpProperty prop;
	prop.id = 0;
	prop.name = tr(u8"name");
	prop.desc = tr(u8"fast fourier transform");
	prop.val = u8"fft";
	prop.flag = k_readonly;
	ps.push_back(prop);
	
	return ps;
}


void KcFftOp::onPropertyChanged(int id, const QVariant& newVal)
{

}


kRange KcFftOp::range(kIndex axis) const
{
	auto objp = dynamic_cast<const KvDataProvider*>(parent());
	assert(objp != nullptr);

	// 对信号的最高2维进行变换，其他低维度保持原信号尺度不变
	if (axis == objp->dim() - 1) {
		auto dx = objp->step(objp->dim() - 1);
		assert(dx != KvData::k_nonuniform_step);
		auto xmax = dx != KvData::k_unknown_step ? 0.5 / dx : 8000; // 未知情况下，显示8khz频率范围
		return { 0, xmax };
	}
	else if (axis == objp->dim()) {
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
		auto xrange = objp->range(objp->dim() - 1);
		auto len = xrange.length();
		return len == 0 ? KvData::k_unknown_step : 1 / len;
	}
	else if (axis == objp->dim()) {
		return KvData::k_nonuniform_step;
	}

	return objp->step(axis);
}


std::shared_ptr<KvData> KcFftOp::processImpl_(std::shared_ptr<KvData> data)
{
	if (data->empty())
		return data;

	return data->dim() == 1 ? process1d(data) : process2d(data); 
}


std::shared_ptr<KvData> KcFftOp::process1d(std::shared_ptr<KvData> data)
{
	assert(data->dim() == 1);

	auto data1d = std::dynamic_pointer_cast<KvData1d>(data);
	assert(data1d && data->step(0) != KvData::k_nonuniform_step);

	if (data1d->count() < 2 || data1d->range(0).empty())
		return data;

	if (rdft_ == nullptr || rdft_->sizeT() != data1d->count())
		rdft_ = std::make_unique<KgRdft>(data1d->count(), false, true);

	auto df = step(0);
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


std::shared_ptr<KvData> KcFftOp::process2d(std::shared_ptr<KvData> data)
{
	assert(data->dim() == 2);

	auto data2d = std::dynamic_pointer_cast<KvData2d>(data);
	assert(data2d && data->step(1) != KvData::k_nonuniform_step);
	assert(KtuMath<kReal>::almostEqualRel(data->step(1) * range(1).length(), kReal(0.5)));

	if (data2d->length(1) < 2 || data2d->range(1).empty())
		return data;

	if (rdft_ == nullptr || rdft_->sizeT() != data2d->length(1))
		rdft_ = std::make_unique<KgRdft>(data2d->length(1), false, true);

	auto df = step(1);
	if (df == KvData::k_unknown_step)
		df = 1 / data2d->range(1).length();

	auto res = std::make_shared<KcSampled2d>();

	res->resize(*data2d);
	res->reset(1, 0, df); // 修正fft变换后的频率参数
	assert(res->length(1) == rdft_->sizeF());

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