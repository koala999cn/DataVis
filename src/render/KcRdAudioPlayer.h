#pragma once
#include "KvDataRender.h"
#include <memory>

class KcAudioRender;

class KcRdAudioPlayer : public KvDataRender
{
public:
	KcRdAudioPlayer(KvDataProvider* is);

	kPropertySet propertySet() const override;

	std::string errorText() const override;

	bool canShown() const override;

	bool isVisible() const override;


public slots:
	void reset() override;

	void show(bool bShow) override;


private:
	void setPropertyImpl_(int id, const QVariant& newVal) override;

	void syncParent() override;

	bool renderImpl_(std::shared_ptr<KvData> data) override;

private:
	std::unique_ptr<KcAudioRender> render_;
	unsigned deviceId_;
	kIndex channels_;
	unsigned sampleRate_;
	double frameTime_;
};
