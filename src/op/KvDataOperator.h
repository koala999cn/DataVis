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

	/// provider接口

	bool isStream() const override;

	kIndex dim() const override;

	kIndex channels() const override;

	kRange range(kIndex axis) const override;

	kReal step(kIndex axis) const override;

	kIndex size(kIndex axis) const override;

	std::shared_ptr<KvData> fetchData(kIndex outPort) const override;

protected:
	std::vector<std::shared_ptr<KvData>> inputs_{ inPorts() };
	std::vector<std::shared_ptr<KvData>> outputs_{ outPorts() };
};

