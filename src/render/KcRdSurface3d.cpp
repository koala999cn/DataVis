#include "KcRdSurface3d.h"
#include <QtDataVisualization/Q3DSurface.h>
#include <QtDataVisualization/QSurface3DSeries.h>
#include <QtDataVisualization/QSurfaceDataProxy.h>
#include "KvSampled.h"
#include "KtSampler.h"


KcRdSurface3d::KcRdSurface3d(KvDataProvider* is)
	: KvRdQtDataVis(is, "surface3d")
{
	auto surface = new Q3DSurface;
	auto series = new QSurface3DSeries;
	series->setDrawMode(QSurface3DSeries::DrawSurfaceAndWireframe);
	series->setFlatShadingEnabled(true);
	surface->addSeries(series);

	graph3d_ = surface;
	xAxis_ = surface->axisX();
	yAxis_ = surface->axisY(); 
	zAxis_ = surface->axisZ();

	syncAxes_();
}


KcRdSurface3d::~KcRdSurface3d()
{
	// TODO: qt是否会自动释放？
	auto surface = dynamic_cast<Q3DSurface*>(graph3d_);
	auto sl = surface->seriesList();
	for (auto s : sl) {
		surface->removeSeries(s);
		delete s;
	}
}


KcRdSurface3d::kPropertySet KcRdSurface3d::propertySet() const
{
	kPropertySet ps = KvRdQtDataVis::propertySet();

	return ps;
}


void KcRdSurface3d::reset()
{
	auto surface = dynamic_cast<Q3DSurface*>(graph3d_);
	auto sl = surface->seriesList();
	for (auto s : sl)
		s->dataProxy()->arrayReset();
}


void KcRdSurface3d::setPropertyImpl_(int id, const QVariant& newVal)
{
	KvRdQtDataVis::setPropertyImpl_(id, newVal);
}


bool KcRdSurface3d::renderImpl_(std::shared_ptr<KvData> data)
{
	std::shared_ptr<KvSampled> samp;

	if (data->isContinued()) {
		samp = std::make_shared<KtSampler<2>>(std::dynamic_pointer_cast<KvContinued>(data));
		samp->reset(0, xAxis_->min(), (xAxis_->max() - xAxis_->min()) / 100, 0.5);
		samp->reset(1, yAxis_->min(), (yAxis_->max() - yAxis_->min()) / 100, 0.5);
	}
	else {
		samp = std::dynamic_pointer_cast<KvSampled>(data);
	}

	auto surface = dynamic_cast<Q3DSurface*>(graph3d_);
	auto series = surface->seriesList().front();

	auto dataArray = new QSurfaceDataArray;
	dataArray->reserve(samp->size(0));
	for (kIndex i = 0; i < samp->size(0); i++) {
		auto newRow = new QSurfaceDataRow(samp->size(1));
		for (kIndex j = 0; j < samp->size(1); j++) {
			auto pt = samp->point(i, j, 0);
			(*newRow)[j++].setPosition(QVector3D(pt[0], pt[1], pt[2]));
		}
		*dataArray << newRow;
	}

	series->dataProxy()->resetArray(dataArray);

	return true;
}


void KcRdSurface3d::syncParent()
{

}

