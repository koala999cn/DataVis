#include "KcFft.h"
#include "KgRdft.h"
#include "KvData1d.h"
#include <assert.h>
#include "KcSampled1d.h"
#include "KtuMath.h"


KcFft::KcFft(KvDataProvider* prov)
	: KvDataOperator("fft", prov) 
{

}


KcFft::kPropertySet KcFft::propertySet() const
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


void KcFft::onPropertyChanged(int id, const QVariant& newVal)
{

}


kRange KcFft::range(kIndex axis) const
{
	auto objp = dynamic_cast<const KvDataProvider*>(parent());
	assert(objp != nullptr);

	if (axis == 0) {
		auto dx = objp->step(0);
		assert(dx != KvData::k_nonuniform_step);
		auto xmax = dx != KvData::k_unknown_step ? 0.5 / dx : 8000; // 未知情况下，显示8khz频率范围
		return { 0, xmax };
	}
	else if (axis == 1) {
		auto r = KvDataOperator::range(1);
		return { 0, KtuMath<kReal>::absMax(r.low(), r.high()) };
	}
	
	return KvDataOperator::range(axis);
}


kReal KcFft::step(kIndex axis) const
{
	if (axis == 0) {
		auto objp = dynamic_cast<const KvDataProvider*>(parent());
		assert(objp != nullptr);

		auto xrange = objp->range(0);
		auto len = xrange.length();
		return len == 0 ? KvData::k_unknown_step : 1 / len;
	}

	return KvData::k_nonuniform_step;
}


std::shared_ptr<KvData> KcFft::processImpl_(std::shared_ptr<KvData> data)
{
	if (1 == data->dim()) {

		auto data1d = std::dynamic_pointer_cast<KvData1d>(data);
		assert(data1d);
		
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
		
		for (kIndex c = 0; c < data1d->channels(); c++) {
			std::vector<kReal> rawData(data1d->count());
			for (kIndex i = 0; i < data1d->count(); i++)
				rawData[i] = data1d->value(i, c).y;

			rdft_->forward(rawData.data());
			rdft_->powerSpectrum(rawData.data());
			res->setChannel(rawData.data(), c);
		}

		return res;
	}
	else { 
		// TODO: 高维FFT
		assert(false);
	}

	return {};
}

