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


void KvPlottable::setData(const_data_ptr d) 
{
	assert(d);

	data_ = d;

	if (d->isContinued() && d->dim() != sampCount_.size())
		sampCount_.assign(d->dim(), std::pow(1000., 1. / d->dim()));
	
	if (data_ && colorMappingDim_ > data_->dim()) 
		setColorMappingDim(data_->dim());

	updateColorMappingPalette();

	dataChanged_ = true;
}


void KvPlottable::setColorMappingDim(unsigned d)
{
	if (data_ && d > data_->dim())
		d = data_->dim();

	if (colorMappingDim_ != d && coloringChanged_ == 0)
		coloringChanged_ = 1;

	colorMappingDim_ = d;
}


void KvPlottable::fitColorMappingRange()
{
	if (data_) {
		assert(colorMappingDim() <= data_->dim());

		auto d = colorMappingDim();
		std::pair<float_t, float_t> newRange;
		if (d < 2 || d == 2 && !usingDefaultZ_()) {
			auto r = boundingBox();
			newRange = { r.lower()[d], r.upper()[d] };
		}
		else {
			auto r = data_->range(d);
			newRange = { r.low(), r.high() };
		}

		if (colorMappingRange_ != newRange) {
			colorMappingRange_ = newRange;
			if (coloringChanged_ == 0)
				coloringChanged_ = 1;
		}
	}
}


bool KvPlottable::empty() const 
{
	return !data_ || data_->size() == 0;
}


KvPlottable::aabb_t KvPlottable::calcBoundingBox_() const
{
	if (empty())
		return aabb_t(); // ����null������Ӱ������ϵ���� 

	point3 lower, upper;

	auto r0 = data_->range(0);
	auto r1 = data_->range(1);

	lower.x() = r0.low(), upper.x() = r0.high();
	lower.y() = r1.low(), upper.y() = r1.high();

	if (usingDefaultZ_()) {
		// ���ù���lower.z��upper.z�Ĵ�С��aabb���캯�����Զ�������Сֵ
		lower.z() = defaultZ(0);
		upper.z() = defaultZ(data_->channels() - 1); // TODO: �˴��̶�ʹ��ͨ����������zƽ����������ɿ������û�����
	}
	else {
		auto r2 = data_->range(2);
		lower.z() = r2.low(), upper.z() = r2.high();
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

	// ������ɫ�����Ķ�̬�仯
	// NB: �����������ݣ����û���̬����sampCount������ͬʱ������ɫ��������ı仯����bars2d��
	// �˴�����̫���ˣ�legend��calcSize��ʱ���Ҫ����һ���Լ��
	// Ŀǰ��4��ͬ����һ�Ǵ˴�������setData��������setColoringMode��������legend��calcSize_��.
	// TODO: �Ƿ��и��Ż��ķ���
	const_cast<KvPlottable*>(this)->updateColorMappingPalette();

	// TODO: ����ϸ�Ŀ��ơ�coloringChanged_��Χ̫����ʵֻ����dimMapping�仯�ǲ������¼��㡣
	if (autoColorMappingRange_ && (dataChanged_ || coloringChanged_))
		const_cast<KvPlottable*>(this)->fitColorMappingRange();

	paint->enableFlatShading(flatShading());
	drawDiscreted_(paint, disc.get());

	dataChanged_ = false;
	coloringChanged_ = 0;
}


std::shared_ptr<const KvDiscreted> KvPlottable::discreted_() const
{
	auto disc = std::dynamic_pointer_cast<const KvDiscreted>(data_);
	if (disc == nullptr) {
		auto cont = std::dynamic_pointer_cast<const KvContinued>(data_);
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
		: ( empty() ? 0 :data_->channels() );
}


unsigned KvPlottable::majorColors() const
{
	return grad_.size();
}


color4f KvPlottable::majorColor(unsigned idx) const
{
	return grad_.at(idx).second;
}


void KvPlottable::setMajorColors(const std::vector<color4f>& majors)
{
	assert(majorColorsNeeded() == -1 || majorColorsNeeded() == majors.size());

	grad_.clear();

	std::vector<float_t> vals(majors.size());
	KuMath::linspace<float_t>(0, 1, 0, vals.data(), majors.size()); // ��ʼ��ʱ���ȼ������
	for (unsigned i = 0; i < majors.size(); i++)
		grad_.insert(vals[i], majors[i]);

	if (coloringChanged_ == 0)
		coloringChanged_ = 1;
}


void KvPlottable::setColoringMode(KeColoringMode mode)
{
	if (coloringMode_ == mode)
		return;

	if (coloringMode_ == k_one_color_solid || mode == k_one_color_solid)
		coloringChanged_ = 2; // ��ģ��ɵ�ɫ��Ϊ��ɫ�����ɲ�ɫ��Ϊ��ɫ
	else
		coloringChanged_ = 1; // С�ģ���ɫ֮��ı仯

	coloringMode_ = mode;
	updateColorMappingPalette();
}


color4f KvPlottable::mapValueToColor_(float_t* valp, unsigned channel) const
{
	switch (coloringMode_)
	{
	case k_one_color_solid:
		return majorColor(channel);

	case k_one_color_gradiant:
		return KuMath::remap(valp[colorMappingDim_],
			colorMappingRange_.first, colorMappingRange_.second,
			majorColor(channel), majorColor(channel).brighten(brightenCoeff_));

	case k_two_color_gradiant:
		return KuMath::remap(valp[colorMappingDim_],
			colorMappingRange_.first, colorMappingRange_.second,
			majorColor(channel), minorColor());

	case k_colorbar_gradiant:
		return grad_.map(KuMath::remap<float_t, true>(valp[colorMappingDim_],
			colorMappingRange_.first, colorMappingRange_.second));

	default:
		assert(false);
		return majorColor(channel);
	}
}


void KvPlottable::updateColorMappingPalette()
{
	// Ĭ�ϵ�ɫ�壨dracula��
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

		unsigned i = majors.size();
		for (; i < std::min<unsigned>(majorColorsNeeded(), std::size(pals)); i++)
			majors.push_back(pals[i]);

		// ��������������ɫ
		if (majors.size() < majorColorsNeeded()) {
			KgRand r;
			while (majors.size() < majorColorsNeeded())
				majors.push_back(color4f(r.rand(0., 1.), r.rand(0., 1.), r.rand(0., 1.), 1));
		}

		majors.resize(majorColorsNeeded());
		setMajorColors(majors);

		if (coloringChanged_ == 0)
			coloringChanged_ = 1;
	}
	else if (majorColorsNeeded() == -1 && majorColors() < 2) {
		std::vector<color4f> majors(std::begin(pals), std::end(pals));
		setMajorColors(majors);

		if (coloringChanged_ == 0)
			coloringChanged_ = 1;
	}
}


bool KvPlottable::usingDefaultZ_() const
{
	return forceDefaultZ_ || (data_ && data_->dim() == 1);
}


void KvPlottable::setSampCount(unsigned dim, unsigned c)
{
	assert(dim < sampCount_.size());
	sampCount_[dim] = c;
	dataChanged_ = true;
}


void KvPlottable::setGradient(const gradient_t& grad) 
{ 
	grad_ = grad;
	if (coloringChanged_ == 0)
		coloringChanged_ = 1;
}


void KvPlottable::setDefaultZ(float_t z) 
{ 
	defaultZ_ = z;
	if (usingDefaultZ_())
		dataChanged_ = true;
}


void KvPlottable::setStepZ(float_t step)
{ 
	stepZ_ = step;
	if (usingDefaultZ_() && data_ && data_->channels() > 1) // ��Ӱ�쵥ͨ������
		dataChanged_ = true;
}


void KvPlottable::setForceDefaultZ(bool b) 
{ 
	forceDefaultZ_ = b;
	if (data() && data()->dim() > 1) // ��Ӱ��һά����
		dataChanged_ = true;
}


void KvPlottable::setFlatShading(bool b)
{
	flatShading_ = b;
	// TODO: vbo���ͬ����״̬��
}


void KvPlottable::setBrightenCoeff(float coeff)
{ 
	brightenCoeff_ = coeff;
	if (coloringMode_ == k_one_color_gradiant && coloringChanged_ == 0)
		coloringChanged_ = 1;
}


void KvPlottable::setColorMappingRange(const std::pair<float_t, float_t>& r)
{
	colorMappingRange_ = r;
	if (coloringMode_ != k_one_color_solid && coloringChanged_ == 0)
		coloringChanged_ = 1;
}


void KvPlottable::setMinorColor(const color4f& minor)
{
	setMinorColor_(minor);
	if (coloringMode_ == k_two_color_gradiant && coloringChanged_ == 0)
		coloringChanged_ = 1;
}