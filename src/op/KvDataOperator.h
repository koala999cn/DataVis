#pragma once
#include "KvDataProvider.h"
#include <memory>

class KvData;


// 数据处理算子的抽象类
// 为了保持接口一致性，此处统一使用KvData进行数据交互
// 当算子是2元时，输入KvData必须是偶通道的，前半通道代表第1个参数，后半通道代表第2个参数
// 当算子是3元时，输入KvData通道数必须是3n，前1/3通道代表第1个参数，中1/3通道代表第2个参数，后1/3通道代表第3个参数
// 依此类推...

class KvDataOperator : public KvDataProvider
{
	Q_OBJECT

public:
	KvDataOperator(const QString& name, KvDataProvider* parent)
		: KvDataProvider(name, parent) {}

	// 实现基类的virtual方法. 默认实现均依赖于parent的行为

	bool isStream() const override {
		return dynamic_cast<const KvDataProvider*>(parent())->isStream();
	}

	unsigned dim() const override {
		return dynamic_cast<const KvDataProvider*>(parent())->dim();
	}

	kRange range(int axis) const override {
		return dynamic_cast<const KvDataProvider*>(parent())->range(axis);
	}

	kReal step(int axis) const override {
		return dynamic_cast<const KvDataProvider*>(parent())->step(axis);
	}

	bool pushData() override {
		return dynamic_cast<KvDataProvider*>(parent())->pushData();
	}

	/// 声明自己的virtual方法接口

	// 输入元：返回1代表输入是一元参数，返回2代表输入是二元参数
	virtual unsigned ins() const = 0;

	// 输出元，同上
	virtual unsigned outs() const = 0;


	/// 执行operator操作
	void process(std::shared_ptr<KvData> data) {
		auto res = processImpl_(data);
		emit onData(res);
	}

signals:
	// 通告数据处理过程，@percent表示完成率，取值[0, 1]，对于长时间任务有用
	// 须由processImpl_发送该信号
	void onProcessing(float percent); 


protected:
	// 返回的KvDataProvider对象由processImpl_创建，由调用者释放
	virtual std::shared_ptr<KvData> processImpl_(std::shared_ptr<KvData> data) = 0;
};

