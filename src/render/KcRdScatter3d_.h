#pragma once
#include "KvRdQwtPlot3d.h"

namespace Qwt3D { class Enrichment; }

class KcRdScatter3d_ : public KvRdQwtPlot3d
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

	void syncAxes_();

	std::vector<Qwt3D::Enrichment*> elist_;
};