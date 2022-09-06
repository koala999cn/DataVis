#include "KvRdPlot3d_.h"
#include "QtAppEventHub.h"
#include "plot/KsPlotManager.h"
#include "plot/KvPlot.h"


KvRdPlot3d_::KvRdPlot3d_(KvDataProvider* is, const QString& name)
    : KvDataRender(name, is)
{
	options_ = k_show;
	plot3d_.reset(KsPlotManager::singleton().createPlot3d());
}


KvRdPlot3d_::KvRdPlot3d_(KvDataProvider* is)
	: KvRdPlot3d_(is, "plot3d")
{

}


KvRdPlot3d_::~KvRdPlot3d_()
{
	setOption(k_show, false);
}


void KvRdPlot3d_::setOption(KeObjectOption opt, bool on)
{
	assert(opt == k_show);
	
	if (on) {
		auto widget = (QWidget*)plot3d_->widget();
		kAppEventHub->showDock(this, widget);
	}
	else
		kAppEventHub->closeDock(this);

	plot3d_->show(on);

	if (on)
		plot3d_->update();
}


bool KvRdPlot3d_::getOption(KeObjectOption opt) const
{
	assert(opt == k_show);
	return plot3d_->visible();
}


namespace kPrivate
{

}


KvRdPlot3d_::kPropertySet KvRdPlot3d_::propertySet() const
{
	using namespace kPrivate;

	kPropertySet ps;

	return ps;
}


void KvRdPlot3d_::setPropertyImpl_(int id, const QVariant& newVal)
{
	using namespace kPrivate;

}


void KvRdPlot3d_::syncAxes_()
{
	auto objp = dynamic_cast<KvDataProvider*>(parent());
	assert(objp);

}


void KvRdPlot3d_::setSize(int axis, int newSize)
{
	assert(axis < 2);

}