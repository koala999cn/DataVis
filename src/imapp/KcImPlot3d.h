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

	color4f background() const override {
		return bkclr_;
	}
	void setBackground(const color4f& clr) override {
		bkclr_ = clr;
	}

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

private:

	void updateImpl_() override;

	void autoProject_() override;

	// 响应鼠标事件，实现旋转、平移、缩放变换
	void handleMouseInput_();

private:
	KtCamera<float> camera_;
	color4f bkclr_{ 1, 1, 1, 1 };

	KtTrackballController<float> trackball_; // 用于操控orient_
};
