#include "KvRdQwtPlot3d.h"
#include "qwtplot3d/qwt3d_plot3d.h"
#include "QtAppEventHub.h"


KvRdQwtPlot3d::KvRdQwtPlot3d(KvDataProvider* is, const QString& name)
	: KvDataRender(name ,is)
{
	options_ = k_show;
}


KvRdQwtPlot3d::~KvRdQwtPlot3d()
{

}


KvRdQwtPlot3d::kPropertySet KvRdQwtPlot3d::propertySet() const
{
	kPropertySet ps;

	return ps;
}


void KvRdQwtPlot3d::setOption(KeObjectOption opt, bool on)
{
	assert(opt == k_show);
	if (on) {
		auto c = connect(kAppEventHub, &QtAppEventHub::isFloatingChanged, [this](KvPropertiedObject* obj, bool) {
			if (obj == this)
				plot3d_->updateData();
			});

		connect(kAppEventHub, &QtAppEventHub::dockClosed, [this, c](KvPropertiedObject* obj) {
			if (obj == this)
				disconnect(c);
			});

		kAppEventHub->showDock(this, plot3d_.get());
	}
	else 
		kAppEventHub->closeDock(this);
}


bool KvRdQwtPlot3d::getOption(KeObjectOption opt) const
{
	assert(opt == k_show);
	return plot3d_->isVisible();
}


QString KvRdQwtPlot3d::exportAs()
{
	return "";
}


void KvRdQwtPlot3d::showData()
{

}


void KvRdQwtPlot3d::rescaleAxes()
{

}


void KvRdQwtPlot3d::setPropertyImpl_(int id, const QVariant& newVal)
{

}