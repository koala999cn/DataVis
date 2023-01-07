#pragma once


class KvPlot;

class KuPlotContextMenu
{
public:

	static void open();

	static void update(KvPlot*);

private:
	KuPlotContextMenu() = delete;
};