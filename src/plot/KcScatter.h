#pragma once
#include "KvPlottable1d.h"
#include "KpContext.h"


// …¢µ„Õº

class KcScatter : public KvPlottable1d
{
	using super_ = KvPlottable1d;

public:

	using super_::super_;

	const color4f& minorColor() const override;

	void setMinorColor_(const color4f& minor) override;

	unsigned objectCount() const override;

	const KpMarker& marker() const { return marker_; }
	void setMarker(const KpMarker&);

private:

	bool objectVisible_(unsigned objIdx) const override;

	void setObjectState_(KvPaint*, unsigned objIdx) const override;

	void* drawObject_(KvPaint*, unsigned objIdx) const override;

protected:
	KpMarker marker_;
};
