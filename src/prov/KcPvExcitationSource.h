#pragma once
#include "KvDataProvider.h"

class KvExcitor;

// 激励源
class KcPvExcitationSource : public KvDataProvider
{
public:

	KcPvExcitationSource();

	bool isStream() const final { return repeat_; }

	kIndex dim() const final { return 1; }

	kIndex channels() const final { return 1; }

	kRange range(kIndex axis) const final;

	kReal step(kIndex axis) const final;

	kIndex size(kIndex axis) const final;

	bool isRunning() const override;

	kPropertySet propertySet() const override;


private:
	bool startImpl_() final;
	bool stopImpl_() final;

	void setPropertyImpl_(int id, const QVariant& newVal) override;

	void timerEvent(QTimerEvent* event) override;

	void createExcitor_();
	void emitData_();

private:
	std::shared_ptr<KvExcitor> excitor_;
	int type_; // 随机分布类型
	kReal duration_; // in second
	kReal step_; // in second
	bool repeat_; // 重复发送数据
	int timerId_;
};
