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

	color4f background() const override {
		return bkclr_;
	}
	void setBackground(const color4f& clr) override {
		bkclr_ = clr;
	}

	void fitData() override {}

private:

	void updateImpl_() override;

private:

	color4f bkclr_{ 1, 1, 1, 1 };
};
