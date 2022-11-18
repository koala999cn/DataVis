#pragma once
#include "prov/KvDataProvider.h"
#include <vector>


// 数据处理算子的抽象类

class KvDataOperator : public KvDataProvider
{
public:
	using KvDataProvider::KvDataProvider;

	/// node接口

	// 默认输入输出端口均为1
	unsigned inPorts() const override { return 1; }

	// 主要前期fetchData，解决输出数据规范的问题（provider接口方法）
	bool onNewLink(KcPortNode* from, KcPortNode* to) override;

	void onDelLink(KcPortNode* from, KcPortNode* to) override;

	void onInput(KcPortNode* outPort, unsigned inPort) override;

	// 作一些一致性检测工作
	bool onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins) override;

	/// provider接口

	std::shared_ptr<KvData> fetchData(kIndex outPort) const override;

	// 获取数据规格的方法：若output有效，优先从output获取；否则从inputs_获取

	int spec(kIndex outPort) const override;

	kRange range(kIndex outPort, kIndex axis) const override;

	kReal step(kIndex outPort, kIndex axis) const override;

	kIndex size(kIndex outPort, kIndex axis) const override;


	/// 自定义接口

	// inPort输入端口是否接受dataSpec规格的数据 ？ 
	virtual bool permitInput(int dataSpec, unsigned inPort) const = 0;

protected:

	/// 几个获取输入数据规格的帮助函数

	// 特定输入端口
	int inputSpec_(kIndex inPort) const;
	kRange inputRange_(kIndex inPort, kIndex axis) const;
	kReal inputStep_(kIndex inPort, kIndex axis) const;
	kIndex inputSize_(kIndex inPort, kIndex axis) const;

	// 首个非空输入端口
	int inputSpec_() const;
	kRange inputRange_(kIndex axis) const;
	kReal inputStep_(kIndex axis) const;
	kIndex inputSize_(kIndex axis) const;

protected:
	std::vector<std::shared_ptr<KvData>> idata_{ inPorts() };
	std::vector<std::shared_ptr<KvData>> odata_{ outPorts() };
	std::vector<KcPortNode*> inputs_{ inPorts() };
};

