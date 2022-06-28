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

	bool doStart() override;

	void doStop() override;


public slots:
	void reset() override;


private:
	void setPropertyImpl_(int id, const QVariant& newVal) override;

	void preRender_() override;

	bool doRender_(std::shared_ptr<KvData> data) override;


private:
	std::unique_ptr<KcAudioRender> render_;
	unsigned deviceId_;
	double frameTime_;
};
