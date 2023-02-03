#pragma once
#include <memory>
#include <string_view>

class KvModule;


// �ṩģ��֮���������ϵ����������������ϵ���г�ʼ���ͷ���ʼ��
// ʹ��ͼ�ı߱�ʾ������ϵ�����ģ��A������ģ��B�������һ����A��B�ı�
// ��ͼ������������֮����ɰ�������г�ʼ��
class KgModuleManager
{
public:
	using module_ptr = std::shared_ptr<KvModule>;

	KgModuleManager();
	~KgModuleManager();

	void registerModule(module_ptr m);

	template<typename T, typename... ARGS>
	std::shared_ptr<T> registerModule(ARGS... args) {
		auto m = std::make_shared<T>(std::forward<ARGS>(args)...);
		registerModule(m);
		return m;
	}

	// ����ģ��A����ģ��B
	void setDependent(module_ptr A, module_ptr B);

	void setDependent(const std::string_view& nameA, const std::string_view& nameB);

	// ģ��A�Ƿ�������ģ��B
	bool isDependent(module_ptr A, module_ptr B);

	unsigned moduleCount() const;

	// get module by name
	module_ptr getModule(const std::string_view& name) const;

	// get module by index
	module_ptr getModule(unsigned idx) const;
	
	// get module by type
	template<typename T>
	std::shared_ptr<T> getModule() const {
		for (unsigned i = 0; i < moduleCount(); i++) {
			auto m = getModule(i);
			if (std::dynamic_pointer_cast<T>(m))
				return std::dynamic_pointer_cast<T>(m);
		}

		return {};
	}

	bool initialize();

	void deinitialize();

private:
	unsigned getVertexId_(module_ptr m) const;
	unsigned getVertexId_(const std::string_view& name) const;

private:
	void* moduleGraph_; // ģ��������ϵͼ
};
