#include "KcPvSampled.h"
#include "KvData.h"
#include <QPointF>
#include "KcSampled1d.h"
#include "KtuMath.h"
#include <assert.h>


KcPvSampled::KcPvSampled(const QString& name, std::shared_ptr<KvData> data)
	: KvDataProvider(name), data_(data) 
{
	assert(data->isSampled());
}


KvPropertiedObject::kPropertySet KcPvSampled::propertySet() const
{
	KvPropertiedObject::kPropertySet ps;

	// TODO: 处理多维和多通道
	if (data_->dim() == 1) {
		auto data = dynamic_cast<KvData*>(data_.get());

		KvPropertiedObject::KpProperty count;
		count.name = u8"Amount";
		count.desc = u8"count of data points";
		count.val = QVariant(data->count());
		count.flag = KvPropertiedObject::k_readonly;
		ps.push_back(count);

		KvPropertiedObject::KpProperty channels;
		channels.name = u8"Channels";
		channels.desc = u8"number of data channels";
		channels.val = QVariant(data->channels());
		channels.flag = KvPropertiedObject::k_readonly;
		ps.push_back(channels);

		if (data->isSampled()) {
			KvPropertiedObject::KpProperty rate;
			rate.name = u8"rate";
			rate.disp = u8"Sampling rate";
			rate.desc = u8"sampling rate of this data";
			rate.val = 1 / data->step(0);
			rate.flag = KvPropertiedObject::k_readonly;
			ps.push_back(rate);
		}

		KvPropertiedObject::KpProperty x, y;
		x.name = u8"min";
		x.desc = u8"minimum value of data keys";
		y.name = u8"max";
		y.desc = u8"maximum value of data keys";

		KvPropertiedObject::KpProperty xrange;
		xrange.name = u8"xrange";
		xrange.disp = u8"Keys";
		xrange.desc = u8"properties of data keys";
		auto xpt = data->range(0);
		xrange.val = QVariant(QPointF(xpt.low(), xpt.high()));
		xrange.flag = KvPropertiedObject::k_readonly;
		xrange.children.push_back(x); 
		xrange.children.push_back(y);
		ps.push_back(xrange);

		KvPropertiedObject::KpProperty yrange;
		yrange.name = u8"yrange";
		yrange.disp = u8"Values";
		yrange.desc = u8"properties of data values";
		auto ypt = data->range(1);
		yrange.val = QVariant(QPointF(ypt.low(), ypt.high()));
		yrange.flag = KvPropertiedObject::k_readonly;
		x.desc = u8"minimum value of data values";
		y.desc = u8"maximum value of data values";
		yrange.children.push_back(x);
		yrange.children.push_back(y);
		ps.push_back(yrange);
	}


	return ps;
}


kIndex KcPvSampled::dim() const
{
	return data_->dim();
}


kIndex KcPvSampled::channels() const
{
	return data_->channels();
}


kRange KcPvSampled::range(kIndex axis) const
{
	return data_->range(axis);
}


kReal KcPvSampled::step(kIndex axis) const
{
	return data_->step(axis); 
}


kIndex KcPvSampled::length(kIndex axis) const
{
	return data_->length(axis);
}


bool KcPvSampled::startImpl_()
{
	emit onData(data_);
	return true;
}


bool KcPvSampled::stopImpl_()
{
	return true; // TODO
}

bool KcPvSampled::running() const
{
	return false;
}