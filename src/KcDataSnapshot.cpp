#include "KcDataSnapshot.h"
#include "KvData1d.h"
#include <QPointF>
#include "KcSampled1d.h"
#include "KcFormulaData1d.h"


QString KcDataSnapshot::typeText() const
{
	switch (type_) {
	case k_sampled:   return u8"采样数据";
	case k_scattered: return u8"散列数据";
	case k_continued: return u8"连续数据";
	}

	return u8"未知";
}


KvPropertiedObject::kPropertySet KcDataSnapshot::propertySet() const
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
			rate.val = type() == k_sampled ? dynamic_cast<KcSampled1d*>(data)->samplingRate()
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
		auto xpt = data->xrange();
		xrange.val = QVariant(QPointF(xpt.first, xpt.second));
		xrange.flag = KvPropertiedObject::k_readonly;
		xrange.children.push_back(x); 
		xrange.children.push_back(y);
		ps.push_back(xrange);

		KvPropertiedObject::KpProperty yrange;
		yrange.name = u8"yrange";
		yrange.disp = u8"Values";
		yrange.desc = u8"properties of data values";
		auto ypt = data->yrange();
		yrange.val = QVariant(QPointF(ypt.first, ypt.second));
		yrange.flag = KvPropertiedObject::k_readonly;
		x.desc = u8"minimum value of data values";
		y.desc = u8"maximum value of data values";
		yrange.children.push_back(x);
		yrange.children.push_back(y);
		ps.push_back(yrange);
	}


	return ps;
}