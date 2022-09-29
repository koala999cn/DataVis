#pragma once
#include <string_view>
#include <memory>
#include <list>

class KvImWindow;

// 管理所有创建的im窗口（KvImWindow派生类实例）
// 把窗口分为两类：一为静态，二为动态
// 
// 静态窗口，有且仅有1个实例，且名字唯一，生命周期贯穿程序全程
// 动态窗口，可有多个实例，生命周期不定，随时响应用户操作而开闭
// 
// 静态窗口管理为考虑同步，窗口注册须在程序初始化阶段完成，不能进入update时再注册
// 动态窗口一般在update阶段注册，为实现同步，使用了延迟队列queued_，新注册窗口先添加到此队列，
// 在下一个update周期前，再统一从queued_复制到dynamics_队列

class KgImWindowManager
{
public:
	using window_ptr = std::shared_ptr<KvImWindow>;

	KgImWindowManager();
	~KgImWindowManager();

	void registerStatic(window_ptr inst); 

	template<typename T, typename... ARGS>
	std::shared_ptr<T> registerStatic(ARGS... args) {
		auto w = std::make_shared<T>(std::forward<ARGS>(args)...);
		registerStatic(w);
		return w;
	}

	window_ptr getStatic(const std::string_view& name);

	template<typename T> std::shared_ptr<T> getStatic() {
		for (auto& i : statics_)
			if (std::dynamic_pointer_cast<T>(i))
				return std::dynamic_pointer_cast<T>(i);

		return {};
	}

	void registerDynamic(window_ptr inst); // 注册窗口到指定分组，分组名由参数group确定
	
	template<typename T, typename... ARGS>
	std::shared_ptr<T> registerDynamic(ARGS... args) {
		auto w = std::make_shared<T>(std::forward(args)...);
		registerDynamic(w);
		return w;
	}

	void releaseStatic(window_ptr inst);
	void releaseStatic(const std::string_view& name);

	void releaseDynamic(window_ptr inst);


	void showMenu(const std::string_view& menuName);

	void update(); // draw all registered imgui_windows

	void showAll(); // 显示所有窗口
	void closeAll(); // 关闭所有窗口，实际是隐藏而非销毁

	void releaseAll(); // 释放所有窗口实例，可在程序退出时调用. 析构函数也会自动调用该方法。

private:
	void setVisible_(bool b);

private:

	// imgui内置窗口的显示
	bool showImGuiBuiltins_{ true };
	bool showDemo_{ false };
	bool showDebugLog_{ false };
	bool showMetrics_{ false };
	bool showStackTool_{ false };
	bool showAbout_{ false };

	std::list<window_ptr> statics_;
	std::list<window_ptr> dynamics_;
	std::list<window_ptr> registerQueue_;
	std::list<window_ptr> releaseQueue_;
};
