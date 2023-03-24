#include "KcScatter.h"
#include "plot/KvPaint.h"
#include "KvData.h"


unsigned KcScatter::objectCount() const
{ 
	return empty() ? 0 : channels_();
} 


bool KcScatter::objectVisible_(unsigned objIdx) const
{
	return marker_.visible();
}


void KcScatter::setObjectState_(KvPaint* paint, unsigned objIdx) const
{
	paint->apply(marker_);
	if (coloringMode() == k_one_color_solid)
		paint->setColor(majorColor(objIdx));
}


void* KcScatter::drawObject_(KvPaint* paint, unsigned objIdx) const
{
	auto g = KuDataUtil::pointsAt(discreted_(), objIdx);

	if (coloringMode() == k_one_color_solid) {
		return paint->drawMarkers(toPoint3Getter_(g.getter, objIdx), g.size);
	}
	else {
		auto coloring = [g, objIdx, this](unsigned i) {
			auto val = g.getter(i);
			return mapValueToColor_(val.data(), objIdx);
		};

		return paint->drawMarkers(toPoint3Getter_(g.getter, objIdx), coloring, nullptr, g.size);
	}

	return nullptr; 
}


const color4f& KcScatter::minorColor() const
{
	return marker_.outline;
}


void KcScatter::setMinorColor_(const color4f& minor)
{
	marker_.outline = minor;
}
