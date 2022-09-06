#include "KsPlotManager.h"
#include <vlCore/VisualizationLibrary.hpp> // 基于VisualizationLibrary实现


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