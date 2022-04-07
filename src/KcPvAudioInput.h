#pragma once
#include "KvDataStream.h"


class KcPvAudioInput : public KvDataStream
{
public:
	KcPvAudioInput();
	virtual ~KcPvAudioInput();

	bool pushData() final;
	void stop() final;
	bool running() const final;

	kIndex dim() const final { return 1; };

	kRange range(kIndex axis) const final;

	kReal step(kIndex axis) const final;

	kPropertySet propertySet() const override;

	kIndex channels() const override { return channels_; }

	auto sampleRate() const { return sampleRate_; }


private:
	void setPropertyImpl_(int id, const QVariant& newVal) override;


private:
	void* dptr_;
	unsigned deviceId_;
	kIndex channels_;
	unsigned sampleRate_;
	double frameTime_;
};

