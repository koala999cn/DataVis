#pragma once
#include <string_view>
#include <memory>
#include <unordered_map>
#include <vector>

class KvImWindow;

// �������д�����im���ڣ�KvImWindow������ʵ����

class KgImWindowManager
{
public:
	using window_ptr = std::shared_ptr<KvImWindow>;
	using window_group = std::vector<window_ptr>;

	KgImWindowManager();
	~KgImWindowManager();

	// �������ע�ᴰ��
	// �����ڰ��շ���������鲻��Ӱ�촰�ڵ����ԣ���showMenu�ᰴ�շ���Դ����б���й���
	void registerInstance(window_ptr instance); // ע�ᴰ�ڵ�ȱʡ���飬������Ϊ"_default_"
	void registerInstance(window_ptr instance, const std::string_view& group); // ע�ᴰ�ڵ�ָ�����飬�������ɲ���groupȷ��

	void releaseInstance(window_ptr instance);
	void releaseInstance(const std::string_view& name);

	void showMenu(const std::string_view& menuName);

	void draw(); // draw all registered imgui_windows

	void showAll(); // ��ʾ���д���
	void closeAll(); // �ر����д��ڣ�ʵ�������ض�������

	void releaseAll(); // �ͷ����д���ʵ�������ڳ����˳�ʱ����. ��������Ҳ���Զ����ø÷�����

private:
	void setVisible_(bool b);

private:

	// imgui���ô��ڵ���ʾ
	bool showDemo_{ false };
	bool showMetrics_{ false };
	bool showDebugLog_{ false };
	bool showStackTool_{ false };
	
	std::unordered_map<std::string, window_group> groups_;
};
