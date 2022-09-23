#pragma once
#include <string_view>
#include <vector>


// 可用于KcImNodeEditor的节点抽象类
// 内置id, name和port管理实现

class KvNode
{
public:
	KvNode(const std::string_view& name, unsigned ins, unsigned outs);

	int id() const { return id_; }

	const std::string& name() const { return name_; }

	unsigned ins() const { return ins_.size(); }
	unsigned outs() const { return outs_.size(); }

	bool hasIn() const { return !ins_.empty(); }
	bool hasOut() const { return !outs_.empty(); }

	// 第idx个输入端口的id
	int inId(unsigned idx) const { return std::get<0>(ins_[idx]); }

	// 第idx个输入端口的flag
	int inFlag(unsigned idx) const { return std::get<1>(ins_[idx]); }
	int& inFlag(unsigned idx) { return std::get<1>(ins_[idx]); }

	// 第idx个输入端口的名字
	const std::string_view& inName(unsigned idx) const { return std::get<2>(ins_[idx]); }
	std::string& inName(unsigned idx) { return std::get<2>(ins_[idx]); }

	// 第idx个输出端口的id
	int outId(unsigned idx) const { return std::get<0>(outs_[idx]); }

	// 第idx个输出端口的flag
	int outFlag(unsigned idx) const { return std::get<1>(outs_[idx]); }
	int& outFlag(unsigned idx) { return std::get<1>(outs_[idx]); }

	// 第idx个输出端口的名字
	const std::string_view& outName(unsigned idx) const { return std::get<2>(outs_[idx]); }
	std::string& outName(unsigned idx) { return std::get<2>(outs_[idx]); }

protected:
	int id_; // // 用于表示node的唯一id
	std::string name_;
	using port_type = std::tuple<int, int, std::string>; // 端口由id, flag和name
	std::vector<port_type> ins_, outs_; // 输入输出端口
};
