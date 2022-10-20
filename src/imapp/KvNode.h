#pragma once
#include <vector>
#include <memory>
#include "KtUniObject.h"

// 用于KcImNodeEditor的节点抽象类
// 实现端口管理功能

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

	// 输入端口数量
	virtual unsigned inPorts() const = 0;

	// 输出端口数量
	virtual unsigned outPorts() const = 0;

	// 返回true表示接受，返回false表示拒绝新链接
	virtual bool onNewLink(KcPortNode* from, KcPortNode* to) { return true; }

	virtual void onDelLink(KcPortNode* from, KcPortNode* to) { }

	// 以下虚拟接口用来处理流水线操作

	// @ins: 各pair元素表示：<当前节点的输入端口序号, 链接该端口的输出端口指针>
	virtual bool onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins) { 
		return true; }

	virtual void onStopPipeline() {}

	virtual void onNewFrame(int frameIdx) {}
	virtual void onEndFrame(int frameIdx) {}

	// outPort输出端口，已准备好向this节点的第inPort个输入端口传送数据
	virtual void onInput(KcPortNode* outPort, unsigned inPort) = 0;

	// 当this节点的所有输入端口均已接收数据之后，调用该方法产生输出
	// 该方法后，this节点的所有输出端口须全部ready
	virtual void output() = 0;

	virtual void showProperySet();
};


class KcPortNode : public KvNode
{
public:
	enum KeType { k_in, k_out };

	// 构造parent节点的第index个type类型端口
	KcPortNode(KeType type, std::weak_ptr<KvBlockNode> parent, unsigned index);

	KeType type() const { return type_; }

	std::weak_ptr<KvBlockNode> parent() { return parent_; }

	unsigned index() const { return index_; }

private:

	// 构造缺省的端口名字
	static std::string portName_(KeType type, unsigned index);

private:
	KeType type_;
	std::weak_ptr<KvBlockNode> parent_;
	unsigned index_;
};
