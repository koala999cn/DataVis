#include "KvRdQtDataVis.h"
#include <QtDataVisualization/QAbstract3DGraph.h>
#include "QtAppEventHub.h"


KvRdQtDataVis::KvRdQtDataVis(KvDataProvider* is, const QString& name)
    : KvDataRender(name, is)
{
	options_ = k_visible;
	graph3d_ = nullptr;
}


KvRdQtDataVis::~KvRdQtDataVis()
{
	setOption(k_visible, false);
	delete graph3d_;
}


void KvRdQtDataVis::setOption(KeObjectOption opt, bool on)
{
	assert(opt == k_visible);
	if (on)
		kAppEventHub->showDock(this, graph3d_);
	else
		kAppEventHub->closeDock(this);
}


bool KvRdQtDataVis::getOption(KeObjectOption opt) const
{
	assert(opt == k_visible);
	assert(graph3d_);

	return graph3d_->isVisible();
}
