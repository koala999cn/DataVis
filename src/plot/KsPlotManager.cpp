#include "KsPlotManager.h"
#include <vlCore/VisualizationLibrary.hpp> // 基于VisualizationLibrary实现
#include "KcVlPlot3d.h"


bool KsPlotManager::init()
{
	/* init Visualization Library */
	vl::VisualizationLibrary::init();
	return vl::VisualizationLibrary::isCoreInitialized()
		&& vl::VisualizationLibrary::isGraphicsInitialized();
}


KsPlotManager::~KsPlotManager()
{
	/* shutdown Visualization Library */
	vl::VisualizationLibrary::shutdown();
}


KvPlot* KsPlotManager::createPlot3d(QWidget* parent)
{
	return 0;
	//return new KcVlPlot3d(parent);
}
