#include "KcPvDataSnapshot.h"
#include "KvData1d.h"
#include <QPointF>
#include "KcSampled1d.h"
#include "KcFormulaData1d.h"
#include "KtuMath.h"


QString KcPvDataSnapshot::typeText() const
{
	switch (type_) {
	case k_sampled:   return u8"采样数据";
	case k_scattered: return u8"散列数据";
	case k_continued: return u8"连续数据";
	}

	return u8"未知";
}


KvPropertiedObject::kPropertySet KcPvDataSnapshot::propertySet() const
{
	KvPropertiedObject::kPropertySet ps;

	if (data_->dim() == 1) {
		auto data = dynamic_cast<KvData1d*>(data_.get());

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

		if (type() != k_scattered) {
			KvPropertiedObject::KpProperty rate;
			rate.name = u8"rate";
			rate.disp = u8"Sampling rate";
			rate.desc = u8"sampling rate of this data";
			rate.val = type() == k_sampled ? dynamic_cast<KcSampled1d*>(data)->sampleRate()
				: dynamic_cast<KcFormulaData1d*>(data)->samplingRate();
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


kIndex KcPvDataSnapshot::dim() const
{
	return data_->dim();
}


kIndex KcPvDataSnapshot::channels() const
{
	return data_->channels();
}


kRange KcPvDataSnapshot::range(kIndex axis) const
{
	return data_->range(axis);
}


kReal KcPvDataSnapshot::step(kIndex axis) const
{
	return data_->step(axis); 
}


kIndex KcPvDataSnapshot::length(kIndex axis) const
{
	return data_->length(axis);
}


bool KcPvDataSnapshot::pushData()
{
	emit onData(data_);
	return true;
}