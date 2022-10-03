#pragma once
#include "KvImGuizmoWindow.h"
#include "plot/KvPlot.h"


class KcImPlot3d : public KvImGuizmoWindow, public KvPlot
{
public:

	void update() override {
		return KvImGuizmoWindow::update();
	}


private:

};
