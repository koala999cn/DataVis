#pragma once
#include <memory>
#include "KvImWindow.h"

class KvDataProvider;
class KvData;

class KcImDataView : public KvImWindow
{
public:
	KcImDataView(std::shared_ptr<KvDataProvider> prov);
	KcImDataView(std::shared_ptr<KvData> data, std::string_view sv);

	void updateImpl_() override;

private:
	std::weak_ptr<KvDataProvider> prov_;
	std::shared_ptr<KvData> data_;
};