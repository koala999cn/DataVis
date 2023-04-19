#pragma once
#include "KvImWindow.h"
#include "plot/KvPlot3d.h"
#include "KtCamera.h"
#include "KtTrackballController.h"


class KcImPlot3d : public KvImWindow, public KvPlot3d
{
public:

	KcImPlot3d(const std::string_view& name);

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

	mat4 viewMatrix() const override {
		return camera_.viewMatrix();
	}
	void setViewMatrix(const mat4& vm) override {
		camera_.viewMatrix() = vm;
	}

	mat4 projMatrix() const override {
		return camera_.projMatrix();
	}
	void setProjMatrix(const mat4& pm) override {
		camera_.projMatrix() = pm;
	}

	const KtCamera<float_t>& camera() const { return camera_; }

private:

	void updateImpl_() override;

	// 响应鼠标事件，实现旋转、平移、缩放变换
	void handleMouseInput_();

private:
	KtCamera<float_t> camera_;
	
	KtTrackballController<float_t> trackball_; // 用于操控orient_
};
