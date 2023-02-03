#include "KgModuleManager.h"
#include "kgl/GraphX.h"
#include "kgl/core/KtTopologySort.h"
#include <assert.h>
#include "KvModule.h"


namespace kPrivate
{
	// 考虑模块数量不会太大，使用稠密有向图结构来表征模块依赖关系网
	using module_graph_type = DigraphDx<bool, KgModuleManager::module_ptr>;
}


KgModuleManager::KgModuleManager()
{
	moduleGraph_ = new kPrivate::module_graph_type;
}


KgModuleManager::~KgModuleManager()
{
	delete static_cast<kPrivate::module_graph_type*>(moduleGraph_);
}


void KgModuleManager::registerModule(module_ptr m)
{
	auto g = static_cast<kPrivate::module_graph_type*>(moduleGraph_);

	assert(getVertexId_(m) == -1); // 确保模块名字唯一，不重复注册
	g->addVertex(m);
}


void KgModuleManager::setDependent(module_ptr A, module_ptr B)
{
	auto g = static_cast<kPrivate::module_graph_type*>(moduleGraph_);
	auto vA = getVertexId_(A);
	auto vB = getVertexId_(B);
	g->addEdge(vA, vB);
}


void KgModuleManager::setDependent(const std::string_view& nameA, const std::string_view& nameB)
{
	auto g = static_cast<kPrivate::module_graph_type*>(moduleGraph_);
	auto vA = getVertexId_(nameA);
	auto vB = getVertexId_(nameB);
	g->addEdge(vA, vB);
}


bool KgModuleManager::isDependent(module_ptr A, module_ptr B)
{
	auto g = static_cast<kPrivate::module_graph_type*>(moduleGraph_);
	auto vA = getVertexId_(A);
	auto vB = getVertexId_(B);
	return g->hasEdge(vA, vB);
}


unsigned KgModuleManager::moduleCount() const
{
	auto g = static_cast<kPrivate::module_graph_type*>(moduleGraph_);
	return g->order();
}


KgModuleManager::module_ptr KgModuleManager::getModule(const std::string_view& name) const
{
	auto g = static_cast<const kPrivate::module_graph_type*>(moduleGraph_);
	for (unsigned i = 0; i < moduleCount(); i++) {
		if (getModule(i)->name() == name)
			return getModule(i);
	}

	return {};
}


KgModuleManager::module_ptr KgModuleManager::getModule(unsigned idx) const
{
	auto g = static_cast<const kPrivate::module_graph_type*>(moduleGraph_);
	return g->vertexAt(idx);
}


bool KgModuleManager::initialize()
{
	auto g = static_cast<kPrivate::module_graph_type*>(moduleGraph_);

	auto ts = KtTopologySort<kPrivate::module_graph_type>(*g);
	for (unsigned i = g->order() - 1; i != -1; i--) {
		if (!getModule(ts.relabel(i))->initialize()) {
			// 回退进行deinitialize
			for (unsigned j = i + 1; j < g->order(); j++)
				getModule(ts.relabel(j))->deinitialize();

			return false;
		}
	}

	for (unsigned v = 0; v < moduleCount(); v++)
		if (!getModule(v)->postInitialize()) {
			deinitialize();
			return false;
		}

	return true;
}


void KgModuleManager::deinitialize()
{
	auto g = static_cast<kPrivate::module_graph_type*>(moduleGraph_);

	auto ts = KtTopologySort<kPrivate::module_graph_type>(*g);
	for (unsigned i = 0; i < g->order(); i++) 
		getModule(ts.relabel(i))->deinitialize();
}


unsigned KgModuleManager::getVertexId_(module_ptr m) const
{
	for (unsigned i = 0; i < moduleCount(); i++) {
		if (m == getModule(i))
			return i;
	}

	return -1;
}


unsigned KgModuleManager::getVertexId_(const std::string_view& name) const
{
	for (unsigned i = 0; i < moduleCount(); i++) {
		if (getModule(i)->name() == name)
			return i;
	}

	return -1;
}
