#pragma once
#include "KvImWindow.h"


class KcImDataView : public KvImWindow
{
public:

	using KvImWindow::KvImWindow;

    const char* type() const override { return "DataView"; }

    void update() override;
};
