#pragma once
#include "KvNode.h"
#include "KvData.h"
#include <memory>

class KvImWindow;

// 数据源的抽象类

class KvDataProvider : public KvBlockNode
{
public:

	using KvBlockNode::KvBlockNode;
	
	virtual ~KvDataProvider();

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

	kIndex size() const {
		kIndex c(1);
		for (kIndex i = 0; i < dim(); i++)
			c *= size(i);
		return c;
	}

	bool isContinued() const;

	bool isDiscreted() const;

	bool isScattered() const;

	bool isSeries() const; // 是否序列数据

	bool isSampled() const;

	void onInput(KcPortNode* outPort, unsigned inPort) override;

	// 默认所有数据都已就绪，需要逐帧准备数据的可以重载该方法
	void output() override {}

	// 处理鼠标左键双击：弹出数据窗口，显示当前各端口的输出
	void onDoubleClicked() override;

	// 抓取第outPort个输出端口的数据
	virtual std::shared_ptr<KvData> fetchData(kIndex outPort) = 0;

private:
	std::shared_ptr<KvImWindow> win_; // 挂接的ImWindow
};

