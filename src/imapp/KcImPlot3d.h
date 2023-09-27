#pragma once
#include "KvImWindow.h"
#include "plot/KvPlot3d.h"
#include "KtCamera.h"
#include "KtTrackballController.h"


class KcImPlot3d : public KvImWindow, public KvPlot3d
{
public:

	KcImPlot3d(const std::string_view& name);

	using KvPlot3d::visible;
	using KvPlot3d::name;

	void onVisibleChanged() override {
		KvImWindow::setVisible(visible());
	}

	void onNameChanged() override {
		KvImWindow::name() = name();
	}

	std::string label() const override;

	mat4 viewMatrix() const override {
		return camera().viewMatrix();
	}
	void setViewMatrix(const mat4& vm) override {
		camera().viewMatrix() = vm;
	}

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

	// 响应鼠标事件，实现旋转、平移、缩放变换
	void handleMouseInput_();

private:
	KtTrackballController<float_t> trackball_; // 用于操控orient_
};
