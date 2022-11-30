#pragma once
#include "KvDataProvider.h"

class KvExcitor;
class KvData;


// 激励源
class KcPvExcitor : public KvDataProvider
{
	using super_ = KvDataProvider;

public:

	KcPvExcitor();

	int spec(kIndex outPort) const override;

	kRange range(kIndex outPort, kIndex axis) const final;

	kReal step(kIndex outPort, kIndex axis) const final;

	kIndex size(kIndex outPort, kIndex axis) const final;

	bool onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins) final;

	void onStopPipeline() final;

	void output() final;

	std::shared_ptr<KvData> fetchData(kIndex outPort) const final;

	void showProperySet() override;


private:

	void createExcitor_();

private:
	std::shared_ptr<KvData> data_; // 当前帧生成的数据
	std::shared_ptr<KvExcitor> excitor_;
	int type_{ 0 }; // 随机分布类型

	kReal pluseRate_{ 4000 }; // 信号发生速率：每秒产生的脉冲数
	kIndex plusePerFrame_{ 256 }; // 每帧产生的脉冲数
	kReal timePerFrame_{ 256 / 4000 }; // 每帧（即每次调用fetchData）产生该时长的信号，与pluseRate_和plusePerFrame_联动
};
