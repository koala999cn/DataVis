#include "KcRdBar3d.h"
#include <QtDataVisualization/Q3dBars.h>


KcRdBar3d::KcRdBar3d(KvDataProvider* is)
	: KvRdQtDataVis(is, "bar3d")
{
	auto bars = new Q3DBars;
	series_ = new QBar3DSeries;
	bars->addSeries(series_);
	graph3d_ = bars;


	auto r0 = is->range(0);
	//bars->columnAxis()->setRange(r0.low(), r0.high());
	bars->columnAxis()->setRange(0, is->size(0) - 1);

	if (is->dim() > 1) {
		auto r1 = is->range(1);
		//bars->rowAxis()->setRange(r1.low(), r1.high());
		bars->rowAxis()->setRange(0, is->size(1) - 1);
	}
	else {
		bars->rowAxis()->setRange(0, 0);
	}

	auto rv = is->range(is->dim());
	bars->valueAxis()->setRange(rv.low(), rv.high());
}


KcRdBar3d::~KcRdBar3d()
{
	auto bars = dynamic_cast<Q3DBars*>(graph3d_);
	auto sl = bars->seriesList();
	for (auto s : sl) {
		bars->removeSeries(s);
		delete s;
	}
}


KcRdBar3d::kPropertySet KcRdBar3d::propertySet() const
{
	kPropertySet ps = KvRdQtDataVis::propertySet();

	return ps;
}


void KcRdBar3d::reset()
{
	auto bars = dynamic_cast<Q3DBars*>(graph3d_);
	auto sl =  bars->seriesList();
	for (auto s : sl)
		s->dataProxy()->resetArray();
}


void KcRdBar3d::setPropertyImpl_(int id, const QVariant& newVal)
{
	KvRdQtDataVis::setPropertyImpl_(id, newVal);
}


bool KcRdBar3d::renderImpl_(std::shared_ptr<KvData> data)
{
	auto bars = dynamic_cast<Q3DBars*>(graph3d_);
	series_->dataProxy()->resetArray();

	if (data->dim() == 1) {
		auto disc = std::dynamic_pointer_cast<KvDiscreted>(data);
		auto row = new QBarDataRow;
		for (kIndex i = 0; i < disc->size(); i++) 
			*row << disc->value(i, 0); // TODO: 处理多通道
			
		series_->dataProxy()->addRow(row);
	}


	return true;
}


void KcRdBar3d::syncParent()
{

}

