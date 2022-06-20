#include "KcRdSurface3d.h"
#include <QtDataVisualization/Q3DSurface.h>
#include <QtDataVisualization/QSurface3DSeries.h>
#include <QtDataVisualization/QSurfaceDataProxy.h>
#include "KvSampled.h"


KcRdSurface3d::KcRdSurface3d(KvDataProvider* is)
	: KvRdQtDataVis(is, "surface3d")
{
	auto surface = new Q3DSurface;
	surface->addSeries(new QSurface3DSeries);

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
	assert(data->isDiscreted());
	auto samp = std::dynamic_pointer_cast<KvSampled>(data);

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

