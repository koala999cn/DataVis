#pragma once
#include "KvNode.h"
#include "KvData.h"


// 支持的数据类型
enum KeDataType
{
	k_unknown,
	k_sampled,
	k_scattered,
	k_continued,
	k_array
};


// 数据规范说明
union KpDataSpec
{
	KpDataSpec() : spec(0) {}
	KpDataSpec(int sp) : spec(sp) {}

	int spec;

	struct {
		int stream : 1; // 是否流式数据
		int dynamic : 1; // 是否动态数据，即数据是否动态变化
		int type : 6; // 数据类型: series, sample, sactter, continue, ..., see KeDataType
		int dim : 8; // 数据的维度，再高好像也没什么意义
		int channels : 8; // 256通道??
		int reserve : 8;
	};
};


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

	void onInput(KcPortNode* outPort, unsigned inPort) override;

	// 默认所有数据都已就绪，需要逐帧准备数据的可以重载该方法
	void output() override {}

	// 处理鼠标左键双击：弹出数据窗口，显示当前各端口的输出
	void onDoubleClicked() override;


	// 返回第outPort个输出端口的数据规格
	virtual int spec(kIndex outPort) const = 0;

	// 返回第axis轴的数据视图范围. 
	virtual kRange range(kIndex outPort, kIndex axis) const = 0;

	// 返回第axis轴的步进，即dx, dy, dz...
	// 返回=0表示步进未知，返回inf表示步进非均匀
	virtual kReal step(kIndex outPort, kIndex axis) const = 0;

	virtual kIndex size(kIndex outPort, kIndex axis) const = 0;

	// 抓取第outPort个输出端口的数据
	virtual std::shared_ptr<KvData> fetchData(kIndex outPort) const = 0;


	/// same helper functions

	bool isStream(kIndex outPort) const;

	bool isDynamic(kIndex outPort) const;

	// 数据的维度
	kIndex dim(kIndex outPort) const;

	kIndex channels(kIndex outPort) const;

	kIndex total(kIndex outPort) const;

	bool isContinued(kIndex outPort) const;

	bool isDiscreted(kIndex outPort) const;

	bool isScattered(kIndex outPort) const;

	bool isSeries(kIndex outPort) const; // 是否序列数据

	bool isSampled(kIndex outPort) const;

	bool isArray(kIndex outPort) const;

	static std::string dataTypeStr(int spec);
	
protected:

	// 向管线管理器发送变化消息的帮助函数
	void notifyChanged_();

private:
	int windowId_{ -1 }; // 挂接的ImWindow标识符
};

