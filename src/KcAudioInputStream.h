#pragma once
#include "KvDataStream.h"


class KcAudioInputStream : public KvDataStream
{
public:
	KcAudioInputStream();
	virtual ~KcAudioInputStream();

	bool pushData() final;
	void stop() final;
	bool running() const final;

	unsigned dim() const final { return 1; };

	kRange range(int axis) const final;

	kReal step(int axis) const final;

	kPropertySet propertySet() const override;

	void onPropertyChanged(int id, const QVariant& newVal) override;

	int channels() const { return channels_; }

	double sampleRate() const { return sampleRate_; }

private:
	void* dptr_;
	unsigned deviceId_;
	int channels_;
	unsigned sampleRate_;
	double frameTime_;
};

