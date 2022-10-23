#pragma once
#include <memory>
#include "KvImWindow.h"

class KvDataProvider;

class KcImDataView : public KvImWindow
{
public:
	KcImDataView(std::shared_ptr<KvDataProvider> prov);

	void updateImpl_() override;

private:
	std::weak_ptr<KvDataProvider> prov_;
};