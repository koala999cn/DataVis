#pragma once
#include "KtSingleton.h"

class KvPlot;
class QWidget;

class KsPlotManager
{
public:
	using singleton_type = KtSingleton<KsPlotManager, false, true>;
	friend singleton_type;

	static KsPlotManager& singleton() {
		return *singleton_type::instance();
	}

	bool init();

	void shutdown() {
		singleton_type::destroy();
	}

	//KvPlot* createPlot3d(QWidget* parent = nullptr);

private:

	// 只允许singleton_typed构造和销毁KsPlotManager
	KsPlotManager() = default;
	~KsPlotManager();
};
