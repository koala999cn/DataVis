#include "KcSpectrogram.h"


KcSpectrogram::KcSpectrogram(KvDataProvider* prov)
	: KvDataOperator("Spectrogram", prov)
{

}


KcSpectrogram::kPropertySet KcSpectrogram::propertySet() const
{
	kPropertySet ps;

	return ps;
}


void KcSpectrogram::onPropertyChanged(int id, const QVariant& newVal)
{

}


std::shared_ptr<KvData> KcSpectrogram::processImpl_(std::shared_ptr<KvData> data)
{
	std::shared_ptr<KvData> res;

	return res;
}