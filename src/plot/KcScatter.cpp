#include "KcScatter.h"
#include "plot/KvPaint.h"
#include "KvData.h"
#include <sstream>


void KcScatter::setData(const_data_ptr d)
{
	super_::setData(d);
	dimSizeVarying_ = odim();
	setLabelingDim(odim());
}


unsigned KcScatter::objectCount() const
{ 
	return empty() ? 0 : channels_() + 1/*label*/;
} 


bool KcScatter::objectVisible_(unsigned objIdx) const
{
	return objIdx == channels_() ? showLabel() : marker_.visible();
}


bool KcScatter::objectReusable_(unsigned objIdx) const
{
	if (objIdx == channels_()) {
		return !dataChanged() && !labelChanged();
	}
	else {
		return super_::objectReusable_(objIdx);
	}
}


void KcScatter::setObjectState_(KvPaint* paint, unsigned objIdx) const
{
	if (objIdx == channels_()) {
		paint->apply(label().font);
		paint->setColor(label().color);
	}
	else {
		paint->apply(marker_);
		if (coloringMode() == k_one_color_solid)
			paint->setColor(majorColor(objIdx));
	}
}


void* KcScatter::drawObject_(KvPaint* paint, unsigned objIdx) const
{
	return objIdx == channels_() ? drawLabel_(paint) : drawMarker_(paint, objIdx);
}


void* KcScatter::drawMarker_(KvPaint* paint, unsigned ch) const
{
	auto g = pointsAt_(ch);

	KvPaint::color_getter coloring = nullptr;
	if (coloringMode() != k_one_color_solid) {
		coloring = [g, ch, this](unsigned i) {
			auto val = g.getter(i);
			return mapValueToColor_(val.data(), ch);
		};
	}

	KvPaint::size_getter sizing = nullptr;
	if (sizeVarying_) {
		sizing = [g, this](unsigned i) {
			return mapValueToSize_(g.getter(i)[sizeVaryingDim()]);
		};
	}

	return paint->drawMarkers(toPoint3Getter_(g.getter, ch), coloring, sizing, g.size);
}


void* KcScatter::drawLabel_(KvPaint* paint) const
{
	std::vector<KvPaint::point3> anchors;
	std::vector<std::string> texts;

	auto count = linesTotal_() * sizePerLine_();
	anchors.reserve(count); texts.reserve(count);

	std::ostringstream strm;
	label().formatStream(strm);

	for (unsigned i = 0; i < channels_(); i++) {
		auto g = pointsAt_(i);
		for (unsigned j = 0; j < g.size; j++) {
			auto pt = g.getter(j);
			anchors.push_back(toPoint_(pt.data(), i));
			strm.str("");
			strm << pt[labelingDim()];
			texts.push_back(strm.str());
		}
	}

	assert(anchors.size() == count);
	const_cast<KcScatter*>(this)->labelChanged() = false;
	return paint->drawTexts(anchors, texts, label().align, label().spacing);
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
	if (sizeVarying_) setDataChanged(false);
}


void KcScatter::setSizeVaryingByArea(bool b)
{ 
	varyingByArea_ = b;
	if (sizeVarying_) setDataChanged(false);
}


void KcScatter::setSizeLower(float s)
{
	sizeLower_ = s;
	if (sizeVarying_) setDataChanged(false);
}


void KcScatter::setSizeUpper(float s)
{
	sizeUpper_ = s;
	if (sizeVarying_) setDataChanged(false);
}


KuDataUtil::KpPointGetter1d KcScatter::pointsAt_(unsigned ch) const
{
	auto lineSize = sizePerLine_();
	std::vector<GETTER> lines(linesPerChannel_());

	for (unsigned i = 0; i < lines.size(); i++) {
		auto g = lineAt_(ch, i);
		assert(lineSize == g.size);
		lines[i] = g.getter;
	}

	KuDataUtil::KpPointGetter1d g;
	g.size = lines.size() * lineSize;
	g.getter = [lines, lineSize](unsigned idx) {
		return lines[idx / lineSize](idx% lineSize);
	};

	return g;
}
