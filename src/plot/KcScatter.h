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

	const KpMarker& marker() const { return marker_; }
	KpMarker& marker() { return marker_; }

	const KpPen& linePen() const { return lineCxt_; }
	KpPen& linePen() { return lineCxt_; }

	bool showLine() const { return showLine_; }
	bool& showLine() { return showLine_; }

private:

	unsigned renderObjectsPerBatch_() const override { return 2; } // marker + line

	void setRenderState_(KvPaint*, unsigned objIdx) const override;

	bool showFill_() const override { return true; }

	bool showEdge_() const override { return false; }

	void* drawObjectImpl_(KvPaint*, GETTER, unsigned count, unsigned objIdx) const override;

protected:
	bool showLine_{ false };
	KpPen lineCxt_;
	mutable KpMarker marker_;
};
