#pragma once
#include "KvImWindow.h"
#include "plot/KvPlot2d.h"
#include "KtCamera.h"


class KcImPlot2d : public KvImWindow, public KvPlot2d
{
public:

	KcImPlot2d(const std::string_view& name);

	void setVisible(bool b) override {
		KvImWindow::setVisible(b);
	}

	bool visible() const override {
		return KvImWindow::visible();
	}


private:
	void updateImpl_() override;

private:
	KtCamera<float_type> camera_;
};
