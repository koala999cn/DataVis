#pragma once
#include <vector>
#include <memory>
#include "KtUniObject.h"

// ������KcImNodeEditor�Ľڵ������
// ʵ�ֶ˿ڹ�����

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

	// @ins: ����˿���
	// @outs: ����˿���
	KvNode(const std::string_view& name, unsigned ins, unsigned outs);

	unsigned ins() const { return ins_.size(); }
	unsigned outs() const { return outs_.size(); }

	bool hasIn() const { return !ins_.empty(); }
	bool hasOut() const { return !outs_.empty(); }

	KcPort& inPort(unsigned idx) { return *ins_[idx]; }
	KcPort& outPort(unsigned idx) { return *outs_[idx]; }

private:
	using port_type = std::unique_ptr<KcPort>;
	std::vector<port_type> ins_, outs_; // ��������˿�
};
