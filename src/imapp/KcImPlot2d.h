#pragma once
#include "KvImWindow.h"
#include "plot/KvPlot2d.h"
#include "KtCamera.h"


class KcImPlot2d : public KvImWindow, public KvPlot2d
{
public:

	KcImPlot2d(const std::string_view& name);

	using KvPlot2d::visible;
	using KvPlot2d::name;

	void onVisibleChanged() override {
		KvImWindow::setVisible(visible());
	}

	void onNameChanged() override {
		KvImWindow::name() = name();
	}

	std::string label() const override;

	mat4 projMatrix() const override {
		return camera().projMatrix();
	}
	void setProjMatrix(const mat4& pm) override {
		camera().projMatrix() = pm;
	}

	KtCamera<double>& camera();
	const KtCamera<double>& camera() const;

private:
	void updateImpl_() override;
};
