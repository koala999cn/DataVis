#pragma once
#include "KtSingleton.h"


class KsPlotManager
{
public:
	using singleton_type = KtSingleton<KsPlotManager, false, true>;
	friend singleton_type;

	static KsPlotManager& singleton() {
		return *singleton_type::instance();
	}

	static bool init();

	static void shutdown() {
		singleton_type::destroy();
	}

private:

	// 只允许singleton_typed构造和销毁KsPlotManager
	KsPlotManager() = default;
	~KsPlotManager();
};