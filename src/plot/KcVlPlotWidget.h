#pragma once
#include "KvPlotWidget.h"


class KcVlPlotWidget : public KvPlotWidget
{
public:
	using typename KvPlotWidget::widget_t;

	KcVlPlotWidget(widget_t* parent);

	void show(bool b) override;

	bool visible() const override;

	void draw(KvRenderer*) const override;

	widget_t* widget() const override;

	void update(bool immediately = true) override;

private:
	void* d_ptr_;
};