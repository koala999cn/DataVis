#include "KcRdGrid3d.h"
#include "qwtplot3d/qwt3d_plot3d.h"
#include "qwtplot3d/qwt3d_gridplot.h"
#include "qwtplot3d//qwt3d_function.h"
#include "KvContinued.h"


KcRdGrid3d::KcRdGrid3d(KvDataProvider* is)
	: KvRdQwtPlot3d(is, "grid3d")
{
	plot3d_ = std::make_unique<Qwt3D::GridPlot>();
}


KcRdGrid3d::~KcRdGrid3d()
{

}


KcRdGrid3d::kPropertySet KcRdGrid3d::propertySet() const
{
	kPropertySet ps = KvRdQwtPlot3d::propertySet();

	return ps;
}


void KcRdGrid3d::reset()
{

}


void KcRdGrid3d::setPropertyImpl_(int id, const QVariant& newVal)
{
	KvRdQwtPlot3d::setPropertyImpl_(id, newVal);
}


namespace kPrivate
{
	class DataMaker_ : public Qwt3D::Function
	{
	public:

		DataMaker_(Qwt3D::GridPlot* pw, KvContinued& cont)
			: Qwt3D::Function(*pw), cont_(cont) {}

		double operator()(double x, double y)
		{
			kReal pt[2] = { x, y };
			return cont_.value(pt, 0);
		}

	private:
		KvContinued& cont_;
	};
}


bool KcRdGrid3d::doRender_(std::shared_ptr<KvData> data)
{
	auto cont = std::dynamic_pointer_cast<KvContinued>(data);

	kPrivate::DataMaker_ dm((Qwt3D::GridPlot*)plot3d_.get(), *cont);
	dm.setMesh(100, 100);    // 200000 polys
	dm.setDomain(-1.73, 1.5, -1.5, 1.5);
	dm.setMinZ(-10);
	dm.create();

	plot3d_->setRotation(30, 0, 15);
	plot3d_->setScale(2, 2, 1.2);
	plot3d_->setShift(0.15, 0, 0);
	plot3d_->setZoom(0.6);

	for (unsigned i = 0; i != plot3d_->coordinates()->axes.size(); ++i)
	{
		plot3d_->coordinates()->axes[i].setMajors(7);
		plot3d_->coordinates()->axes[i].setMinors(4);
	}


	plot3d_->coordinates()->axes[Qwt3D::X1].setLabelString("x-axis");
	plot3d_->coordinates()->axes[Qwt3D::Y1].setLabelString("y-axis");
	//coordinates()->axes[Z1].setLabelString(QChar(0x38f)); // Omega - see http://www.unicode.org/charts/


	plot3d_->setCoordinateStyle(Qwt3D::BOX);

	plot3d_->updateData();
	plot3d_->update();

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


void KcRdGrid3d::preRender_()
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
