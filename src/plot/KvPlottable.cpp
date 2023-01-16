#include "KvPlottable.h"
#include "KvDiscreted.h"
#include "KvContinued.h"
#include "KcSampler.h"
#include "KvPaint.h"
#include "KgRand.h"


KvPlottable::KvPlottable(const std::string_view& name)
	: super_(name)
{

}


void KvPlottable::setData(data_ptr d) 
{
	assert(d);

	data_ = d;

	if (d->isContinued() && d->dim() != sampCount_.size())
		sampCount_.assign(d->dim(), std::pow(1000., 1. / d->dim()));
	
	if (data_ && colorMappingDim_ > data_->dim())
		colorMappingDim_ = data_->dim();

	// TODO: 以下代码不宜放此处，主循环的每个更新周期时都会调用setData
	updateColorMappingPalette_();
	resetColorMappingRange();
}


void KvPlottable::setColorMappingDim(unsigned d)
{
	if (data_ && d > data_->dim())
		d = data_->dim();
	colorMappingDim_ = d;
}


void KvPlottable::resetColorMappingRange()
{
	if (data_) {
		assert(colorMappingDim() <= data_->dim());
		auto r = data_->range(colorMappingDim());
		colorMappingRange_ = { r.low(), r.high() };
	}
}


bool KvPlottable::empty() const 
{
	return !data_ || data_->size() == 0;
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

	auto disc = discreted_();
	if (disc == nullptr || disc->empty())
		return;

	paint->enableFlatShading(flatShading());
	drawDiscreted_(paint, disc.get());
}


std::shared_ptr<KvDiscreted> KvPlottable::discreted_() const
{
	auto disc = std::dynamic_pointer_cast<KvDiscreted>(data_);
	if (disc == nullptr) {
		auto cont = std::dynamic_pointer_cast<KvContinued>(data_);
		if (cont) {
			auto samp = std::make_shared<KcSampler>(cont);
			if (samp) {
				for (unsigned i = 0; i < cont->dim(); i++)
					samp->reset(i, cont->range(i).low(), cont->length(i) / sampCount_[i]);
				disc = samp;
			}
		}
	}

	return disc;
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
	assert(majorColorsNeeded() == -1 || majorColorsNeeded() == majors.size());

	colorBar_.reset();

	std::vector<float_t> vals(majors.size());
	KtuMath<float_t>::linspace(0, 1, 0, vals.data(), majors.size()); // 初始化时均匀间隔配置
	for (unsigned i = 0; i < majors.size(); i++)
		colorBar_.setAt(vals[i], majors[i]);
}


void KvPlottable::setColoringMode(KeColoringMode mode)
{
	coloringMode_ = mode;
	updateColorMappingPalette_();
}


color4f KvPlottable::mapValueToColor_(float_t* valp, unsigned channel) const
{
	switch (coloringMode_)
	{
	case k_one_color_solid:
		return majorColor(channel);

	case k_one_color_gradiant:
		return KtuMath<float_t>::remap(valp[colorMappingDim_],
			colorMappingRange_.first, colorMappingRange_.second,
			majorColor(channel), majorColor(channel).brighten(brightenCoeff_));

	case k_two_color_gradiant:
		return KtuMath<float_t>::remap(valp[colorMappingDim_],
			colorMappingRange_.first, colorMappingRange_.second,
			majorColor(channel), minorColor());

	case k_colorbar_gradiant:
		return colorBar_.getAt(KtuMath<float_t>::remap<true>(valp[colorMappingDim_],
			colorMappingRange_.first, colorMappingRange_.second));

	default:
		assert(false);
		return majorColor(channel);
	}
}


void KvPlottable::updateColorMappingPalette_()
{
	// 默认调色板（dracula）
	static color4f pals[] = {
		color3c(255, 85, 85), // #FF5555
		color3c(255, 184, 108), // #FFB86C
		color3c(241, 250, 108), // #F1FA8C
		color3c(80, 250, 123), // #50FA7B
		color3c(189, 147, 249), // #BD93F9
		color3c(139, 233, 253), // #8BE9FD
		color3c(255, 121, 198) // #FF79C6
	};

	if (majorColorsNeeded() != -1 && majorColors() != majorColorsNeeded()) {
		std::vector<color4f> majors;
		for (unsigned i = 0; i < majorColors(); i++)
			majors.push_back(majorColor(i));

		if (majors.size() < majorColorsNeeded())
			majors.insert(majors.end(), std::begin(pals), std::end(pals));

		// 还不够则添加随机色
		if (majors.size() < majorColorsNeeded()) {
			KgRand r;
			while (majors.size() < majorColorsNeeded())
				majors.push_back(color4f(r.rand(0., 1.), r.rand(0., 1.), r.rand(0., 1.), 1));
		}

		majors.resize(majorColorsNeeded());
		setMajorColors(majors);
	}
	else if (majorColorsNeeded() == -1 && majorColors() == 0) {
		std::vector<color4f> majors(std::begin(pals), std::end(pals));
		setMajorColors(majors);
	}
}

