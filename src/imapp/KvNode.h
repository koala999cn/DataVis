#pragma once
#include <string_view>
#include <vector>


// ������KcImNodeEditor�Ľڵ������
// ����id, name��port����ʵ��

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

	// ��idx������˿ڵ�id
	int inId(unsigned idx) const { return std::get<0>(ins_[idx]); }

	// ��idx������˿ڵ�flag
	int inFlag(unsigned idx) const { return std::get<1>(ins_[idx]); }
	int& inFlag(unsigned idx) { return std::get<1>(ins_[idx]); }

	// ��idx������˿ڵ�����
	const std::string_view& inName(unsigned idx) const { return std::get<2>(ins_[idx]); }
	std::string& inName(unsigned idx) { return std::get<2>(ins_[idx]); }

	// ��idx������˿ڵ�id
	int outId(unsigned idx) const { return std::get<0>(outs_[idx]); }

	// ��idx������˿ڵ�flag
	int outFlag(unsigned idx) const { return std::get<1>(outs_[idx]); }
	int& outFlag(unsigned idx) { return std::get<1>(outs_[idx]); }

	// ��idx������˿ڵ�����
	const std::string_view& outName(unsigned idx) const { return std::get<2>(outs_[idx]); }
	std::string& outName(unsigned idx) { return std::get<2>(outs_[idx]); }

protected:
	int id_; // // ���ڱ�ʾnode��Ψһid
	std::string name_;
	using port_type = std::tuple<int, int, std::string>; // �˿���id, flag��name
	std::vector<port_type> ins_, outs_; // ��������˿�
};
