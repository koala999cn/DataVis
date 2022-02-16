#pragma once
#include "KvPropertiedObject.h"


class KvOutputDevice : public KvPropertiedObject
{
public:
	enum KeOutoutType
	{
		k_plot, 
		k_disk_file, 
		k_device, 
	};

	KvOutputDevice(const QString& name, int type)
		: KvPropertiedObject(name), type_(type) {}

	virtual ~KvOutputDevice() {}

	int type() const { return type_; }

private:
	int type_;
};

