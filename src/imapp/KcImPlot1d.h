#pragma once
#include "KvImWindow.h"
#include "plot/KvPlot.h"


class KcImPlot1d : public KvImWindow, public KvPlot
{
public:

	KcImPlot1d(const std::string_view& name);

	void setVisible(bool b) override {
		KvImWindow::setVisible(b);
	}

	bool visible() const override {
		return KvImWindow::visible();
	}

	void fitData() override {}

private:

	void updateImpl_() override;
};
