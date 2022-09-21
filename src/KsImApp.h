#pragma once
#include "KtSingleton.h"


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


private:
	KsImApp();
	~KsImApp();

	KsImApp(const KsImApp& app) = delete;
	void operator=(const KsImApp& app) = delete;

private:
	void* mainWindow_;
};
