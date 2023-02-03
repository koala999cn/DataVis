#pragma once
#include <memory>
#include <string_view>

class KvModule;


// 提供模块之间的依赖关系管理，并按照依赖关系进行初始化和反初始化
// 使用图的边表示依赖关系，如果模块A依赖于模块B，则存在一条从A到B的边
// 对图进行拓扑排序之后，则可按逆序进行初始化
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

	// 设置模块A依赖模块B
	void setDependent(module_ptr A, module_ptr B);

	void setDependent(const std::string_view& nameA, const std::string_view& nameB);

	// 模块A是否依赖于模块B
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
	void* moduleGraph_; // 模块依赖关系图
};
