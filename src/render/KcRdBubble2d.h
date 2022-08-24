#pragma once
#include "KvRdCustomPlot.h"
#include "kDsp.h" // for kReal


class KcRdBubble2d : public KvRdCustomPlot
{
public:
	KcRdBubble2d(KvDataProvider* is);

	kPropertySet propertySet() const override;

	void reset() override;


private:
	void setPropertyImpl_(int id, const QVariant& newVal) override;
	bool doRender_(std::shared_ptr<KvData> data) override;
	void preRender_() override;
};

