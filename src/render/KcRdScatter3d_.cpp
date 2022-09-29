#include "KcRdScatter3d_.h"
#include "plot/KvPlot.h"
#include "plot/KcGraph.h"


KcRdScatter3d_::KcRdScatter3d_(KvDataProvider* is)
	: KvRdPlot3d_(is, "scatter3d")
{
	for(unsigned i = 0; i < is->channels(); i++)
	    plot3d_->addPlottable(new KcGraph);
}


KcRdScatter3d_::~KcRdScatter3d_()
{

}


KcRdScatter3d_::kPropertySet KcRdScatter3d_::propertySet() const
{
	kPropertySet ps = KvRdPlot3d_::propertySet();

	return ps;
}


void KcRdScatter3d_::reset()
{
	
}


void KcRdScatter3d_::setPropertyImpl_(int id, const QVariant& newVal)
{
	KvRdPlot3d_::setPropertyImpl_(id, newVal);
}


bool KcRdScatter3d_::doRender_(std::shared_ptr<KvData> data)
{
	assert(data->channels() == plot3d_->plottableCount());
	assert(data->isDiscreted());
	auto disc = std::dynamic_pointer_cast<KvDiscreted>(data);

	for (kIndex c = 0; c < disc->channels(); c++) {
		auto plot = plot3d_->plottable(c);
		auto graph = dynamic_cast<KcGraph*>(plot);
		assert(graph);

		auto& gdata = graph->data();
		gdata->clear();
		gdata->reserve(disc->size());

		if (data->dim() == 1) {
			for (kIndex i = 0; i < disc->size(); i++) {
				auto pt = disc->pointAt(i, c);
				gdata->push_back({ pt[0], pt[1], 0 });
			}
		}
		else {
			assert(data->dim() == 2);
			for (kIndex i = 0; i < disc->size(); i++) {
				auto pt = disc->pointAt(i, c);
				gdata->push_back({ pt[0], pt[1], pt[2] });
			}
		}
	}

	return true;
}


void KcRdScatter3d_::preRender_()
{
	// 保持图形的数据序列数目与父节点的数据通道数目一致

/*	auto objp = dynamic_cast<KvDataProvider*>(parent());
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
	}*/
}
