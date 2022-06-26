#include "KcRdBar3d.h"
#include <QtDataVisualization/Q3dBars.h>
#include "KvSampled.h"


KcRdBar3d::KcRdBar3d(KvDataProvider* is)
	: KvRdPlot3d(is, "bar3d")
{
	auto bars = new Q3DBars;
	series_ = new QBar3DSeries;
	bars->addSeries(series_);
	graph3d_ = bars;
	xAxis_ = bars->columnAxis();
	yAxis_ = bars->rowAxis();
	zAxis_ = bars->valueAxis();


	// bars3d有特殊的range设置需求，不能调用基类的syncAxes_
	size0_ = is->size(0);
	xAxis_->setRange(0, size0_ - 1);

	size1_ = is->dim() > 1 ? is->size(1) : 1;
	yAxis_->setRange(0, size1_ - 1);

	auto rv = is->range(is->dim());
	bars->valueAxis()->setRange(rv.low(), rv.high());

	connect(xAxis_, &QAbstract3DAxis::rangeChanged, [this](float min, float max) {
		setSize(0, int(max - min) + 1);
		});

	connect(yAxis_, &QAbstract3DAxis::rangeChanged, [this](float min, float max) {
		setSize(1, int(max - min) + 1);
		});
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
	kPropertySet ps = KvRdPlot3d::propertySet();

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
	KvRdPlot3d::setPropertyImpl_(id, newVal);
}


bool KcRdBar3d::doRender_(std::shared_ptr<KvData> data)
{
	auto objp = dynamic_cast<KvDataProvider*>(parent());
	return objp->isStream() ? renderStream_(data) : renderSnap_(data);
}


void KcRdBar3d::preRender_()
{

}


bool KcRdBar3d::renderSnap_(std::shared_ptr<KvData> data)
{
	auto samp = std::dynamic_pointer_cast<KvSampled>(data);
	if (samp == nullptr || data->dim() > 2)
		return false;

	series_->dataProxy()->resetArray();

	// TODO: 处理多通道
	if (data->dim() == 1) {
		auto row = new QBarDataRow;
		auto sz = std::min<kIndex>(size0_, samp->size());
		for (kIndex i = 0; i < sz; i++)
			*row << samp->valueAt(i, 0);

		series_->dataProxy()->addRow(row);
	}
	else if (data->dim() == 2) {
		auto sx = std::min<kIndex>(size0_, samp->size(0));
		auto sy = std::min<kIndex>(size1_, samp->size(1));

		for (kIndex y = 0; y < sy; y++) {
			auto row = new QBarDataRow;
			for (kIndex x = 0; x < sx; x++)
				*row << samp->value(x, y, 0);

			series_->dataProxy()->addRow(row);
		}
	}

	return true;
}


namespace kPrivate
{
	QBarDataRow* copyBarDataRow(const QBarDataRow* row)
	{
		auto newRow = new QBarDataRow;
		if (row) 
			*newRow = *row;

		return newRow;
	}

	QBarDataRow* copySeriesRow(QBar3DSeries* series, int rowIndex)
	{
		return copyBarDataRow(rowIndex < series->dataProxy()->rowCount() ?
			series->dataProxy()->rowAt(rowIndex) : nullptr);
	}
}


bool KcRdBar3d::renderStream_(std::shared_ptr<KvData> data)
{
	auto samp = std::dynamic_pointer_cast<KvSampled>(data);
	if (samp == nullptr || data->dim() > 2)
		return false;

	auto darray = new QBarDataArray;

	// TODO: 处理多通道
	if (data->dim() == 1) {
		auto row = kPrivate::copySeriesRow(series_, 0);
		for (kIndex i = 0; i < samp->size(0); i++)
			*row << samp->valueAt(i, 0);

		int upper = int(xAxis_->max()) + 1;
		if (row->size() > upper)
			row->remove(0, row->size() - upper);

		darray->push_back(row);
	}
	else if (data->dim() == 2) {
		for (kIndex y = 0; y < samp->size(1); y++) {
			auto row = kPrivate::copySeriesRow(series_, y);
			for (kIndex x = 0; x < samp->size(0); x++)
				*row << samp->value(x, y, 0);

			int upper = int(xAxis_->max()) + 1;
			if (row->size() > upper)
				row->remove(0, row->size() - upper);

			darray->push_back(row);
		}
	}

	series_->dataProxy()->resetArray(darray);

	return true;
}