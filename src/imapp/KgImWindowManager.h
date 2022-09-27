#pragma once
#include <string_view>
#include <memory>
#include <list>

class KvImWindow;

// �������д�����im���ڣ�KvImWindow������ʵ����
// �Ѵ��ڷ�Ϊ���ࣺһΪ��̬����Ϊ��̬
// 
// ��̬���ڣ����ҽ���1��ʵ����������Ψһ���������ڹᴩ����ȫ��
// ��̬���ڣ����ж��ʵ�����������ڲ�������ʱ��Ӧ�û�����������
// 
// ��̬���ڹ���Ϊ����ͬ��������ע�����ڳ����ʼ���׶���ɣ����ܽ���updateʱ��ע��
// ��̬����һ����update�׶�ע�ᣬΪʵ��ͬ����ʹ�����ӳٶ���queued_����ע�ᴰ������ӵ��˶��У�
// ����һ��update����ǰ����ͳһ��queued_���Ƶ�dynamics_����

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

	void registerDynamic(window_ptr inst); // ע�ᴰ�ڵ�ָ�����飬�������ɲ���groupȷ��
	
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

	void showAll(); // ��ʾ���д���
	void closeAll(); // �ر����д��ڣ�ʵ�������ض�������

	void releaseAll(); // �ͷ����д���ʵ�������ڳ����˳�ʱ����. ��������Ҳ���Զ����ø÷�����

private:
	void setVisible_(bool b);

private:

	// imgui���ô��ڵ���ʾ
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
