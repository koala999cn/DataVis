#include "KvPlottable2d.h"
#include "KuDataUtil.h"
#include "KvDiscreted.h"


void KvPlottable2d::setData(const_data_ptr d)
{
	bool updateDimMapping = !idata() || idata()->dim() != d->dim();

	super_::setData(d);

	// µ÷ÕûÎ¬¶ÈÓ³Éä
	if (updateDimMapping) {
		setXdim(odim() - 2);
		setYdim(odim() - 1);
		setZdim(odim());
	}
}


const color4f& KvPlottable2d::minorColor() const
{
	return borderPen_.color;
}


void KvPlottable2d::setMinorColor_(const color4f& minor)
{
	borderPen_.color = minor;
}


unsigned KvPlottable2d::objectCount() const
{
	return 1;
}


bool KvPlottable2d::objectVisible_(unsigned objIdx) const
{
	return filled_ || (showBorder() && borderPen().visible());
}


void KvPlottable2d::setObjectState_(KvPaint* paint, unsigned objIdx) const
{
	bool edged = showBorder() && borderPen().visible();
	paint->setFilled(filled_);
	paint->setEdged(edged);

	if (edged) {
		paint->apply(borderPen());
		paint->setSecondaryColor(borderPen().color);
	}

	if (coloringMode() == k_one_color_solid)
		paint->setColor(majorColor(objIdx));
}


unsigned KvPlottable2d::gridsPerChannel_() const
{
	return linesPerChannel_() / linesPerGrid_();
}


unsigned KvPlottable2d::gridsTotal_() const
{
	return gridsPerChannel_() * channels_();
}


unsigned KvPlottable2d::linesPerGrid_() const
{
	return discreted_()->size(odim() - 2);
}


KuDataUtil::KpPointGetter1d KvPlottable2d::gridLineAt_(unsigned ch, unsigned gridIdx, unsigned lineIdx) const
{
	return lineAt_(ch, gridIdx * linesPerGrid_() + lineIdx);
}
