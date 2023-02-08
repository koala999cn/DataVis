#pragma once
#include "KvDataProvider.h"

class KvExcitor;
class KvData;


// 激励源provider的抽象类

class KvPvExcitor : public KvDataProvider
{
	using super_ = KvDataProvider;

public:

	using super_::super_;

	int spec(kIndex outPort) const override;

	kRange range(kIndex outPort, kIndex axis) const override;

	kReal step(kIndex outPort, kIndex axis) const override;

	kIndex size(kIndex outPort, kIndex axis) const override;

	void onNewFrame(int frameIdx) override;

	bool onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins) override;

	void onStopPipeline() override;

	void output() override;

	std::shared_ptr<KvData> fetchData(kIndex outPort) const override;

	unsigned dataStamp(kIndex outPort) const override;

	void showPropertySet() override;


protected:

	virtual unsigned typeCount_() const = 0;
	virtual const char* typeStr_(int type) const = 0;
	virtual KvExcitor* createExcitor_(int type) = 0;

	std::shared_ptr<KvExcitor> excitor() const {
		return excitor_; 
	}

private:
	std::shared_ptr<KvData> data_; // 当前帧生成的数据
	std::shared_ptr<KvExcitor> excitor_;
	int type_{ 0 }; // excitor的类型
	int ticksPerFrame_{ 256 }; // 每帧（即每次调用fetchData）产生的脉冲数
	bool typeChanged_{ false }; // 用于标记excitor类型是否发生变化
};
