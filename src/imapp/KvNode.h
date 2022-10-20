#pragma once
#include <vector>
#include <memory>
#include "KtUniObject.h"

// ����KcImNodeEditor�Ľڵ������
// ʵ�ֶ˿ڹ�����

class KvNode : public KtUniObject<KvNode>
{
public:

	using super_ = KtUniObject<KvNode>;

	using super_::super_;
};

class KcPortNode;

class KvBlockNode : public KvNode
{
public:

	using KvNode::KvNode;

	// ����˿�����
	virtual unsigned inPorts() const = 0;

	// ����˿�����
	virtual unsigned outPorts() const = 0;

	// ����true��ʾ���ܣ�����false��ʾ�ܾ�������
	virtual bool onNewLink(KcPortNode* from, KcPortNode* to) { return true; }

	virtual void onDelLink(KcPortNode* from, KcPortNode* to) { }

	// ��������ӿ�����������ˮ�߲���

	// @ins: ��pairԪ�ر�ʾ��<��ǰ�ڵ������˿����, ���Ӹö˿ڵ�����˿�ָ��>
	virtual bool onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins) { 
		return true; }

	virtual void onStopPipeline() {}

	virtual void onNewFrame(int frameIdx) {}
	virtual void onEndFrame(int frameIdx) {}

	// outPort����˿ڣ���׼������this�ڵ�ĵ�inPort������˿ڴ�������
	virtual void onInput(KcPortNode* outPort, unsigned inPort) = 0;

	// ��this�ڵ����������˿ھ��ѽ�������֮�󣬵��ø÷����������
	// �÷�����this�ڵ����������˿���ȫ��ready
	virtual void output() = 0;

	virtual void showProperySet();
};


class KcPortNode : public KvNode
{
public:
	enum KeType { k_in, k_out };

	// ����parent�ڵ�ĵ�index��type���Ͷ˿�
	KcPortNode(KeType type, std::weak_ptr<KvBlockNode> parent, unsigned index);

	KeType type() const { return type_; }

	std::weak_ptr<KvBlockNode> parent() { return parent_; }

	unsigned index() const { return index_; }

private:

	// ����ȱʡ�Ķ˿�����
	static std::string portName_(KeType type, unsigned index);

private:
	KeType type_;
	std::weak_ptr<KvBlockNode> parent_;
	unsigned index_;
};
