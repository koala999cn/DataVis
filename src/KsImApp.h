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

	void quit(); // 调用该函数后，主循环run将退出

	void shutdown();


public:

	// 回调函数

	// 主循环run开始时回调，返回false结束主循环
	using start_listener = std::function<bool(void)>;

	// 每个run周期回调，返回false退出主循环
	using update_listener = std::function<bool(void)>;

	// 主循环run结束时回调
	using finish_listener = std::function<void(void)>;

	void listenStart(start_listener ls) {
		lsStart_.push_back(ls);
	}

	void listenUpdate(update_listener ls) {
		lsUpdate_.push_back(ls);
	}

	void listenFinish(finish_listener ls) {
		lsFinish_.push_back(ls);
	}

private:
	KsImApp();
	~KsImApp();

	KsImApp(const KsImApp& app) = delete;
	void operator=(const KsImApp& app) = delete;

private:
	void* mainWindow_;

	std::vector<start_listener> lsStart_;
	std::vector<update_listener> lsUpdate_;
	std::vector<finish_listener> lsFinish_;
};
