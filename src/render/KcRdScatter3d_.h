#pragma once
#include "KvRdPlot3d_.h"


class KcRdScatter3d_ : public KvRdPlot3d_
{
public:
	KcRdScatter3d_(KvDataProvider* is);
	virtual ~KcRdScatter3d_();

	kPropertySet propertySet() const override;

	void reset() override;

private:
	void setPropertyImpl_(int id, const QVariant& newVal) override;
	bool doRender_(std::shared_ptr<KvData> data) override;
	void preRender_() override;
};
