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

	void output() override {}

	// 调用permitInput虚拟方法，以判断是否接受该连接
	bool onNewLink(KcPortNode* from, KcPortNode* to) override;

	bool onInputChanged(KcPortNode* outPort, unsigned inPort) override;

	// inPort输入端口是否接受dataSpec规格的数据 ？ 
	virtual bool permitInput(int dataSpec, unsigned inPort) const {
		return true;
	}


protected:

	bool working_() const;
};

