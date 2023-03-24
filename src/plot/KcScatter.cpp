#include "KcScatter.h"
#include "plot/KvPaint.h"
#include "KvData.h"


void KcScatter::setData(const_data_ptr d)
{
	super_::setData(d);
	dimSizeVarying_ = odim();
}


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

	KvPaint::color_getter coloring = nullptr;
	if (coloringMode() != k_one_color_solid) {
		coloring = [g, objIdx, this](unsigned i) {
			auto val = g.getter(i);
			return mapValueToColor_(val.data(), objIdx);
		};
	}

	KvPaint::size_getter sizing = nullptr;
	if (sizeVarying_) {
		sizing = [g, this](unsigned i) {
			return mapValueToSize_(g.getter(i)[sizeVaryingDim()]);
		};
	}

	return paint->drawMarkers(toPoint3Getter_(g.getter, objIdx), coloring, sizing, g.size);;
}


const color4f& KcScatter::minorColor() const
{
	return marker_.outline;
}


void KcScatter::setMinorColor_(const color4f& minor)
{
	marker_.outline = minor;
}


float KcScatter::mapValueToSize_(float_t val) const
{
	auto r = odata()->range(dimSizeVarying_);

	if (!sizeVaryingByArea())
		return KuMath::remap<float_t, true>(val, r.low(), r.high(),
			sizeLower_, sizeUpper_);

	auto factor = KuMath::remap<float_t, true>(val, r.low(), r.high(),
		sizeLower_ * float_t(sizeLower_), sizeUpper_ * float_t(sizeUpper_));

	return std::sqrt(factor);
}


void KcScatter::setSizeVarying(bool b)
{
	sizeVarying_ = b;
	setDataChanged(false);
}


void KcScatter::setSizeVaryingDim(unsigned d)
{
	assert(d <= odim());
	dimSizeVarying_ = d;
	setDataChanged(false);
}


void KcScatter::setSizeVaryingByArea(bool b)
{ 
	varyingByArea_ = b;
	setDataChanged(false);
}


void KcScatter::setSizeLower(float s)
{
	sizeLower_ = s;
	setDataChanged(false);
}


void KcScatter::setSizeUpper(float s)
{
	sizeUpper_ = s;
	setDataChanged(false);
}
