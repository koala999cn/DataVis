#pragma once
#include "KvModule.h"

struct GLFWwindow;

class KcModuleImGuiGlfw : public KvModule
{
public:

	KcModuleImGuiGlfw(int w, int h, const char* title);

	bool initialize() override;

	void deinitialize() override;

	bool postInitialize() override;

	GLFWwindow* glfwWindow() {
		return glfwWin_;
	}

private:
	bool initGl_();
	bool initImGui_();

	void deinitGl_();
	void deinitImGui_();

private:

	// 配置参数
	int width_, height_;
	std::string title_;

	// 初始化生成参数
	std::string glslVer_;
	GLFWwindow* glfwWin_{ nullptr };
};
