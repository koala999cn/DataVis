#pragma once
#include "KvInputStream.h"


class KcAudioInputStream : public KvInputStream
{
public:
	KcAudioInputStream();
	virtual ~KcAudioInputStream();

	bool start() override;
	void stop() override;
	bool running() const override;

	kPropertySet propertySet() const override;

	void onPropertyChanged(int id, const QVariant& newVal) override;

	int channels() const {
		return channels_;
	}

	double sampleRate() const {
		return sampleRate_;
	}

private:
	void* dptr_;
	unsigned deviceId_;
	int channels_;
	unsigned sampleRate_;
	double frameTime_;
};

