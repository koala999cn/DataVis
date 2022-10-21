#pragma once
#include "KvNode.h"
#include <memory>
#include "KvDiscreted.h"


// 数据源的抽象类

class KvDataProvider : public KvBlockNode
{
public:

	using KvBlockNode::KvBlockNode;

	// 数据源没有输入端口
	unsigned inPorts() const override { return 0; }

	// 缺省输出端口数为1
	unsigned outPorts() const override { return 1; }

	void showProperySet() override;

	virtual bool isStream() const = 0;

	// 数据的维度
	virtual kIndex dim() const = 0;

	virtual kIndex channels() const { return 1; }

	// 返回第axis轴的数据视图范围. 
	virtual kRange range(kIndex axis) const = 0;

	// 返回第axis轴的步进，即dx, dy, dz...
	// 返回=0表示步进未知，返回inf表示步进非均匀
	virtual kReal step(kIndex axis) const = 0;

	virtual kIndex size(kIndex axis) const = 0;

	// 实现dim, channels, range, step等基本属性
	//kPropertySet propertySet() const override;

	kIndex size() const {
		kIndex c(1);
		for (kIndex i = 0; i < dim(); i++)
			c *= size(i);
		return c;
	}

	bool isContinued() const {
		return size(0) == KvData::k_inf_size;
	}

	bool isDiscreted() const {
		return size(0) != KvData::k_inf_size;
	}

	bool isScattered() const {
		return isDiscreted() && step(0) == KvDiscreted::k_nonuniform_step;
	}

	// 是否序列数据
	bool isSeries() const {
		return isDiscreted() && step(0) == 1;
	}

	bool isSampled() const {
		return isDiscreted() && step(0) != KvDiscreted::k_nonuniform_step;
	}


	void onInput(KcPortNode* outPort, unsigned inPort) override;

	//默认所有数据都已就绪，需要逐帧准备数据的可以重载该方法
	void output() override {}

	// 抓取第outPort个输出端口的数据
	virtual std::shared_ptr<KvData> fetchData(kIndex outPort) = 0;
};

