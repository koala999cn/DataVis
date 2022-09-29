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


class KvBlockNode : public KvNode
{
public:

	using KvNode::KvNode;

	virtual unsigned inPorts() const = 0;

	virtual unsigned outPorts() const = 0;
};


class KcPortNode : public KvNode
{
public:
	enum KeType { k_in, k_out };

	// 构造parent节点的第index个type类型端口
	KcPortNode(KeType type, std::weak_ptr<KvBlockNode> parent, unsigned index);


private:
	static std::string portName_(KeType type, unsigned index);

private:
	KeType type_;
	std::weak_ptr<KvBlockNode> parent_;
	unsigned index_;
};