#pragma once
#include "KvDataProvider.h"


class KcPvAudioInput : public KvDataProvider
{
public:
	KcPvAudioInput();
	virtual ~KcPvAudioInput();

	bool isStream() const override { return true; }

	kIndex dim() const final { return 1; };

	kRange range(kIndex axis) const final;

	kReal step(kIndex axis) const final;

	kIndex size(kIndex) const final;

	kPropertySet propertySet() const override;

	kIndex channels() const override { return channels_; }

	bool running() const final;

	auto sampleRate() const { return sampleRate_; }


private:
	bool startImpl_() final;
	bool stopImpl_() final;
	void setPropertyImpl_(int id, const QVariant& newVal) override;


private:
	void* dptr_;
	unsigned deviceId_;
	kIndex channels_;
	unsigned sampleRate_;
	double frameTime_;
};

