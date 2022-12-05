#pragma once
#include "KvImWindow.h"
#include "plot/KvPlot2d.h"
#include "KtProjector3d.h"


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

	std::string title() const override {
		return KvImWindow::name();
	}

	std::string& title() override {
		return KvImWindow::name();
	}

	std::string label() const override;

private:
	void updateImpl_() override;

private:
	KtProjector<float_t> camera_;
};
