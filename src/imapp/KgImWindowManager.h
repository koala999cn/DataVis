#pragma once
#include <string_view>
#include <memory>
#include <unordered_map>
#include <vector>

class KvImWindow;

// 管理所有创建的im窗口（KvImWindow派生类实例）

class KgImWindowManager
{
public:
	using window_ptr = std::shared_ptr<KvImWindow>;
	using window_group = std::vector<window_ptr>;

	KgImWindowManager();
	~KgImWindowManager();

	// 向管理器注册窗口
	// 各窗口按照分组管理，分组不会影响窗口的特性，但showMenu会按照分组对窗口列表进行归类
	void registerInstance(window_ptr instance); // 注册窗口到缺省分组，分组名为"_default_"
	void registerInstance(window_ptr instance, const std::string_view& group); // 注册窗口到指定分组，分组名由参数group确定

	void releaseInstance(window_ptr instance);
	void releaseInstance(const std::string_view& name);

	void showMenu(const std::string_view& menuName);

	void draw(); // draw all registered imgui_windows

	void showAll(); // 显示所有窗口
	void closeAll(); // 关闭所有窗口，实际是隐藏而非销毁

	void releaseAll(); // 释放所有窗口实例，可在程序退出时调用. 析构函数也会自动调用该方法。

private:
	void setVisible_(bool b);

private:

	// imgui内置窗口的显示
	bool showDemo_{ false };
	bool showMetrics_{ false };
	bool showDebugLog_{ false };
	bool showStackTool_{ false };
	
	std::unordered_map<std::string, window_group> groups_;
};
