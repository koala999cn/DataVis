#include "KcRdScatter3d.h"
#include <QtDataVisualization/Q3DScatter.h>
#include <QtDataVisualization/QScatter3DSeries.h>


KcRdScatter3d::KcRdScatter3d(KvDataProvider* is)
	: KvRdPlot3d(is, "scatter3d")
{
	auto scatter = new Q3DScatter;

	graph3d_ = scatter;
	xAxis_ = scatter->axisX();
	yAxis_ = scatter->axisY();
	zAxis_ = scatter->axisZ();

	size0_ = is->size(0);
	size1_ = is->dim() > 1 ? is->size(1) : 1;

	syncAxes_();
}


KcRdScatter3d::~KcRdScatter3d()
{
	// TODO: qt是否会自动释放？
	auto scatter = dynamic_cast<Q3DScatter*>(graph3d_);
	auto sl = scatter->seriesList();
	for (auto s : sl) {
		scatter->removeSeries(s);
		delete s;
	}
}


KcRdScatter3d::kPropertySet KcRdScatter3d::propertySet() const
{
	kPropertySet ps = KvRdPlot3d::propertySet();

	return ps;
}


void KcRdScatter3d::reset()
{
	auto scatter = dynamic_cast<Q3DScatter*>(graph3d_);
	auto sl = scatter->seriesList();
	for (auto s : sl)
		s->dataProxy()->arrayReset();
}


void KcRdScatter3d::setPropertyImpl_(int id, const QVariant& newVal)
{
	KvRdPlot3d::setPropertyImpl_(id, newVal);
}


bool KcRdScatter3d::doRender_(std::shared_ptr<KvData> data)
{
	assert(data->isDiscreted());
	auto disc = std::dynamic_pointer_cast<KvDiscreted>(data);

	auto scatter = dynamic_cast<Q3DScatter*>(graph3d_);
	auto sl = scatter->seriesList();

		
	for (kIndex c = 0; c < disc->channels(); c++) {
		QScatterDataArray* dataArray = new QScatterDataArray;
		dataArray->resize(disc->size());
		QScatterDataItem* ptrToDataArray = &dataArray->first();

		if (data->dim() == 1) {
			for (kIndex i = 0; i < disc->size(); i++) {
				auto pt = disc->pointAt(i, c);
				ptrToDataArray->setPosition(QVector3D(pt[0], pt[1], 0));
				ptrToDataArray++;
			}
		}
		else {
			assert(data->dim() == 2);
			for (kIndex i = 0; i < disc->size(); i++) {
				auto pt = disc->pointAt(i, c);
				ptrToDataArray->setPosition(QVector3D(pt[0], pt[1], pt[2]));
				ptrToDataArray++;
			}
		}

		sl.at(c)->dataProxy()->resetArray(dataArray);
	}

	return true;
}


void KcRdScatter3d::preRender_()
{
	// 保持图形的数据序列数目与父节点的数据通道数目一致

	auto objp = dynamic_cast<KvDataProvider*>(parent());
	assert(objp);

	auto scatter = dynamic_cast<Q3DScatter*>(graph3d_);
	auto sl = scatter->seriesList();

	while (sl.size() < objp->channels()) {
		scatter->addSeries(new QScatter3DSeries);
		sl = scatter->seriesList();
	}

	while (sl.size() > objp->channels()) {
		scatter->removeSeries(sl.back());
		delete sl.back();
		sl = scatter->seriesList();
	}
}


void KcRdScatter3d::syncAxes_()
{
	auto objp = dynamic_cast<KvDataProvider*>(parent());
	assert(objp);

	auto rx = objp->range(0);
	auto ry = objp->range(1);
	xAxis_->setRange(rx.low(), rx.high());
	yAxis_->setRange(ry.low(), ry.high());

	if (objp->dim() == 1) { 
		zAxis_->setRange(0, 0);
	}
	else {
		auto rz = objp->range(2);
		zAxis_->setRange(rz.low(), rz.high());
	}
}
