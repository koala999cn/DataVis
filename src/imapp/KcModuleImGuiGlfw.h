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

	// ���ò���
	int width_, height_;
	std::string title_;

	// ��ʼ�����ɲ���
	std::string glslVer_;
	GLFWwindow* glfwWin_{ nullptr };
};
