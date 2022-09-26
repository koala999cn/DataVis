#pragma once
#include "KtSingleton.h"
#include <functional>
#include <vector>


class KsImApp
{
public:
	using singleton_type = KtSingleton<KsImApp, false, true>;
	friend singleton_type;

	static KsImApp& singleton() {
		return *singleton_type::instance();
	}

	bool init(int w, int h, const char* title);
	
	// Main loop
	void run();

	void quit(); // ���øú�������ѭ��run���˳�

	void shutdown();


public:

	// �ص�����

	// ��ѭ��run��ʼʱ�ص�������false������ѭ��
	using start_listener = std::function<bool()>;

	// ÿ��run���ڻص�������false�˳���ѭ��
	using update_listener = std::function<bool()>;

	// ��ѭ��run����ʱ�ص�
	using finish_listener = std::function<void()>;

	void listenStartRun(start_listener ls) {
		lsStart_.push_back(ls);
	}

	void listenPerFrame(update_listener ls) {
		lsUpdate_.push_back(ls);
	}

	void listenFinishRun(finish_listener ls) {
		lsFinish_.push_back(ls);
	}

private:
	KsImApp();
	~KsImApp();

	KsImApp(const KsImApp& app) = delete;
	void operator=(const KsImApp& app) = delete;

	void drawFrame_();

	const char* initGl_(int w, int h, const char* title);
	bool initImGui_(const char* glsl_version);
	bool initImNode_();
	bool initImFileDialog_();

	void shutGl_();
	void shutImGui_();
	void shutImNode_();
	void shutImFileDialog_();

private:
	void* mainWindow_;

	std::vector<start_listener> lsStart_;
	std::vector<update_listener> lsUpdate_;
	std::vector<finish_listener> lsFinish_;
};
