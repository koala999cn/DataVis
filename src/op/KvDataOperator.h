#pragma once
#include "prov/KvDataProvider.h"


// 数据处理算子的抽象类

class KvDataOperator : public KvDataProvider
{
public:
	using KvDataProvider::KvDataProvider;

	// 默认输入输出端口均为1
	unsigned inPorts() const override { return 1; }

	// 实现基类的virtual方法. 默认实现均依赖于parent的行为
#if 0
	bool isStream() const override {
		return dynamic_cast<const KvDataProvider*>(parent())->isStream();
	}

	kIndex dim() const override {
		return dynamic_cast<const KvDataProvider*>(parent())->dim();
	}

	kIndex channels() const override  { 
		return dynamic_cast<const KvDataProvider*>(parent())->channels();
	}

	kRange range(kIndex axis) const override {
		return dynamic_cast<const KvDataProvider*>(parent())->range(axis);
	}

	kReal step(kIndex axis) const override {
		return dynamic_cast<const KvDataProvider*>(parent())->step(axis);
	}

	kIndex size(kIndex axis) const override {
		return dynamic_cast<const KvDataProvider*>(parent())->size(axis);
	}

	bool isRunning() const override {
		return dynamic_cast<const KvDataProvider*>(parent())->isRunning();
	}

public:

	/// 执行operator操作
	void process(std::shared_ptr<KvData> data) {
		preRender_(); // 支持用户实时调整参数，每次都同步父亲对象
		auto res = processImpl_(data);
		if(res) emit pushData(res);
	}

signals:
	// 通告数据处理过程，@percent表示完成率，取值[0, 1]，对于长时间任务有用
	// 须由processImpl_发送该信号
	void onProcessing(float percent); 


protected:

	// parent的属性变更，同步this属性
	virtual void preRender_() = 0;


	// @data: 待处理数据，为null表示本轮此处理结束，相当于flush提示
	virtual std::shared_ptr<KvData> processImpl_(std::shared_ptr<KvData> data) = 0;
#endif
};

