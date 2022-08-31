#include "KcRdScatter3d_.h"
#include "qwtplot3d/qwt3d_plot3d.h"
#include "qwtplot3d/qwt3d_enrichment_std.h"


KcRdScatter3d_::KcRdScatter3d_(KvDataProvider* is)
	: KvRdQwtPlot3d(is, "scatter3d")
{

	//for(kIndex c = 0; c < is->channels(); c++)
	//    elist_.push_back(plot3d_->addEnrichment(Qwt3D::Dot()));
}


KcRdScatter3d_::~KcRdScatter3d_()
{

}


KcRdScatter3d_::kPropertySet KcRdScatter3d_::propertySet() const
{
	kPropertySet ps = KvRdQwtPlot3d::propertySet();

	return ps;
}


void KcRdScatter3d_::reset()
{

}


void KcRdScatter3d_::setPropertyImpl_(int id, const QVariant& newVal)
{
	KvRdQwtPlot3d::setPropertyImpl_(id, newVal);
}


bool KcRdScatter3d_::doRender_(std::shared_ptr<KvData> data)
{
/*	assert(data->isDiscreted());
	auto disc = std::dynamic_pointer_cast<KvDiscreted>(data);

	//auto scatter = dynamic_cast<Q3DScatter*>(graph3d_);
	//auto sl = scatter->seriesList();


	kIndex c = 0;
	//for (kIndex c = 0; c < disc->channels(); c++) {
		//QScatterDataArray* dataArray = new QScatterDataArray;
		//dataArray->resize(disc->size());
		//QScatterDataItem* ptrToDataArray = &dataArray->first();

		if (data->dim() == 1) {
			for (kIndex i = 0; i < disc->size(); i++) {
				auto pt = disc->pointAt(i, c);
			
				plot3d_->addEnrichment(Qwt3D::Dot() (pt[0], pt[1], 0));
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

		//sl.at(c)->dataProxy()->resetArray(dataArray);
	//}
	*/
	return true;
}


void KcRdScatter3d_::preRender_()
{
	// 保持图形的数据序列数目与父节点的数据通道数目一致
/*
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
	}*/
}


void KcRdScatter3d_::syncAxes_()
{/*
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
	}*/
}
