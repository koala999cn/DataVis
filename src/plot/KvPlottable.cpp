#include "KvPlottable.h"
#include "KvDiscreted.h"
#include "KvContinued.h"
#include "KcSampler.h"
#include "KvPaint.h"


KvPlottable::KvPlottable(const std::string_view& name)
	: super_(name)
{
	colorBar_.setAt(0, color4f(0, 0, 0, 1));
	colorBar_.setAt(1, color4f(1, 1, 1, 1));
}


void KvPlottable::setData(data_ptr d) 
{
	data_ = d;

	if (d && d->isContinued() && d->dim() != sampCount_.size())
		sampCount_.assign(d->dim(), std::pow(1000., 1. / d->dim()));
	
	fitColorMappingRange();
}


void KvPlottable::fitColorMappingRange()
{
	if (coloringMode_ == k_colorbar_gradiant && data_) {
		auto r = data_->valueRange(); // TODO: 支持定制的维度
		colorMappingRange_ = { r.low(), r.high() };
	}
}


bool KvPlottable::empty() const 
{
	return !data_ || data_->size() == 0 
		|| data_->isContinued() && std::find(sampCount_.begin(), sampCount_.end(), 0) != sampCount_.cend();
}


KvPlottable::aabb_t KvPlottable::boundingBox() const
{
	if (empty())
		return aabb_t(point3(0), point3(1)); 

	point3 lower, upper;

	auto r0 = data_->range(0);
	auto r1 = data_->range(1);

	lower.x() = r0.low(), upper.x() = r0.high();
	lower.y() = r1.low(), upper.y() = r1.high();

	if (data_->dim() > 1) {
		auto r2 = data_->range(2);
		lower.z() = r2.low(), upper.z() = r2.high();
	}
	else {
		lower.z() = defaultZ(0);
		upper.z() = defaultZ(data_->channels() - 1);
	}

	return { lower, upper };
}


void KvPlottable::draw(KvPaint* paint) const
{
	assert(paint);

	if (empty())
		return;

	auto d = data();
	for (unsigned i = 0; i < d->dim(); i++)
		if (d->length(i) == 0)
			return;

	auto disc = std::dynamic_pointer_cast<KvDiscreted>(d);
	if (disc == nullptr) {
		auto cont = std::dynamic_pointer_cast<KvContinued>(d);
		assert(cont);

		auto samp = std::make_shared<KcSampler>(cont);
		if (!samp)
			return;
		
		for (unsigned i = 0; i < cont->dim(); i++)
			samp->reset(i, cont->range(i).low(), cont->length(i) / sampCount_[i]);

		disc = samp;
	}

	assert(disc);
	paint->setFlatShading(flatShading());
	drawDiscreted_(paint, disc.get());
}


bool KvPlottable::hasSelfAxis() const
{
	return selfAxes_[0] || selfAxes_[1] || selfAxes_[2];
}


unsigned KvPlottable::majorColorsNeeded() const
{
	return coloringMode_ == k_colorbar_gradiant ? -1
		: ( data_ ? data_->channels() : 1 );
}


unsigned KvPlottable::majorColors() const
{
	return colorBar_.numStops();
}


color4f KvPlottable::majorColor(unsigned idx) const
{
	return colorBar_.stopAt(idx).second;
}


void KvPlottable::setMajorColors(const std::vector<color4f>& majors)
{
	colorBar_.reset();

	std::vector<float_t> vals(majors.size());
	KtuMath<float_t>::linspace(0, 1, 0, vals.data(), majors.size()); // 初始化时均匀间隔配置
	for (unsigned i = 0; i < majors.size(); i++)
		colorBar_.setAt(vals[i], majors[i]);
}


void KvPlottable::setColoringMode(KeColoringMode mode)
{
	coloringMode_ = mode;
	fitColorMappingRange();
}


color4f KvPlottable::mapValueToColor_(float_t val, unsigned channel) const
{
	switch (coloringMode_)
	{
	case k_one_color_solid:
		return majorColor(channel);

	case k_one_color_gradiant:
		return KtuMath<float_t>::remap(val, colorMappingRange_.first, colorMappingRange_.second,
			majorColor(channel), majorColor(channel).brighten(0.75)); // TODO: 亮度可配置

	case k_two_color_gradiant:
		return KtuMath<float_t>::remap(val, colorMappingRange_.first, colorMappingRange_.second,
			majorColor(channel), minorColor());

	case k_colorbar_gradiant:
		return colorBar_.getAt(KtuMath<float_t>::remap<true>(val, 
			colorMappingRange_.first, colorMappingRange_.second));

	default:
		assert(false);
		return majorColor(channel);
	}
}
