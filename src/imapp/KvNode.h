#pragma once
#include <vector>
#include <memory>
#include "KtUniObject.h"

// 可用于KcImNodeEditor的节点抽象类
// 实现端口管理功能

class KvNode : public KtUniObject<>
{
public:

	using super_ = KtUniObject<>;

	class KcPort : public super_
	{
	public:
		KcPort(const std::string_view& name) : super_(name) {}

	private:
		KvNode* parent_{ nullptr };
	};

	// @ins: 输入端口数
	// @outs: 输出端口数
	KvNode(const std::string_view& name, unsigned ins, unsigned outs);

	unsigned ins() const { return ins_.size(); }
	unsigned outs() const { return outs_.size(); }

	bool hasIn() const { return !ins_.empty(); }
	bool hasOut() const { return !outs_.empty(); }

	KcPort& inPort(unsigned idx) { return *ins_[idx]; }
	KcPort& outPort(unsigned idx) { return *outs_[idx]; }

private:
	using port_type = std::unique_ptr<KcPort>;
	std::vector<port_type> ins_, outs_; // 输入输出端口
};
