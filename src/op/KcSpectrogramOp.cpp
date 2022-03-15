#include "KcSpectrogramOp.h"


KcSpectrogramOp::KcSpectrogramOp(KvDataProvider* prov)
	: KvDataOperator("Spectrogram", prov)
{

}


KcSpectrogramOp::kPropertySet KcSpectrogramOp::propertySet() const
{
	kPropertySet ps;

	return ps;
}


void KcSpectrogramOp::onPropertyChanged(int id, const QVariant& newVal)
{

}


std::shared_ptr<KvData> KcSpectrogramOp::processImpl_(std::shared_ptr<KvData> data)
{
	std::shared_ptr<KvData> res;

	return res;
}
