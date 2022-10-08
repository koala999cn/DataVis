#pragma once
#include "KvNode.h"
#include <memory>

class KvData;


// 数据渲染的抽象类
class KvDataRender : public KvBlockNode
{
public:

	// 导入基类构造函数
	using KvBlockNode::KvBlockNode;

	// 缺省输入端口数为1
	unsigned inPorts() const override { return 1; }

	// 渲染器没有输出
	unsigned outPorts() const override { return 0; }

	// 渲染数据
	virtual void pushData(std::shared_ptr<KvData> data) = 0;
};

