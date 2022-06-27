#include "KcPvExcitationSource.h"
#include "KtSampling.h"
#include "excitors.h"
#include "KcSampled1d.h"
#include <chrono>


KcPvExcitationSource::KcPvExcitationSource()
	: KvDataProvider("excitor")
{
	type_ = 0;
	duration_ = 0.1;
	step_ = 0.001;
	repeat_ = true;
	timerId_ = 0;

	createExcitor_();
}


kRange KcPvExcitationSource::range(kIndex axis) const
{
	return axis == 0 ? kRange(0, duration_) : excitor_->range();
}


kReal KcPvExcitationSource::step(kIndex axis) const
{
	return axis == 0 ? step_ : KvDiscreted::k_nonuniform_step;
}


kIndex KcPvExcitationSource::size(kIndex axis) const
{
	if (axis == 0) {
		KtSampling<kReal> samp;
		samp.reset(0, duration_, step_, 0);
		return samp.size();
	}
	else {
		return KvData::k_inf_size;
	}
}


bool KcPvExcitationSource::isRunning() const
{
	return timerId_ != 0;
}


namespace kPrivate
{
	enum KeExcitationType
	{
		k_gauss,
		k_exponent,
		k_laplace,
		k_rayleigh,
		k_lognorm,
		k_cauchy,
		k_weibull
	};


	enum KeExcitationSourceProperty
	{
		k_type,
		k_duration,
		k_step,
		k_repeat,
	};
}


KcPvExcitationSource::kPropertySet KcPvExcitationSource::propertySet() const
{
	kPropertySet ps;
	KpProperty prop;

	static const std::pair<QString, int> type[] = {
		{ "Gauss", kPrivate::k_gauss },
		{ "Exponent", kPrivate::k_exponent },
		{ "Laplace", kPrivate::k_laplace },
		{ "Rayleigh", kPrivate::k_rayleigh },
		{ "Lognorm", kPrivate::k_lognorm },
		{ "Cauchy", kPrivate::k_cauchy },
		{ "Weibull", kPrivate::k_weibull }
	};
	prop.id = kPrivate::k_type;
	prop.name = tr("Distribution");
	prop.val = type_;
	for (unsigned i = 0; i < sizeof(type) / sizeof(std::pair<QString, int>); i++) {
		KvPropertiedObject::KpProperty sub;
		sub.name = type[i].first;
		sub.val = type[i].second;
		prop.children.push_back(sub);
	}
	ps.push_back(prop);

	prop.id = kPrivate::k_duration;
	prop.name = tr("Duration");
	prop.val = duration_;
	prop.minVal = kReal(0);
	prop.maxVal = kReal(1e5);
	prop.step = kReal(0.1);
	prop.children.clear();
	ps.push_back(prop);

	prop.id = kPrivate::k_step;
	prop.name = tr("Step");
	prop.val = step_;
	ps.push_back(prop);

	prop.id = kPrivate::k_repeat;
	prop.name = tr("Repeat");
	prop.val = repeat_;
	ps.push_back(prop);

	return ps;
}


bool KcPvExcitationSource::doStart()
{
	createExcitor_();

	emitData_();

	if (repeat_) {
		timerId_ = startTimer(std::chrono::milliseconds(static_cast<long>(duration_ * 1000)));
		if (timerId_ == 0)
			return false;
	}

	return true;
}


void KcPvExcitationSource::doStop() 
{
	killTimer(timerId_);
	timerId_ = 0;
}


void KcPvExcitationSource::setPropertyImpl_(int id, const QVariant& newVal)
{
	switch (id)
	{
	case kPrivate::k_type:
		type_ = newVal.toInt();
		break;

	case kPrivate::k_duration:
		duration_ = newVal.value<kReal>();
		break;

	case kPrivate::k_step:
		step_ = newVal.value<kReal>();
		break;

	case kPrivate::k_repeat:
		repeat_ = newVal.toBool();
		break;

	default:
		break;
	}
}


void KcPvExcitationSource::createExcitor_()
{
	switch (type_)
	{
	case kPrivate::k_exponent:
		excitor_ = std::make_shared<KcExponentExcitor>();
		break;

	case kPrivate::k_laplace:
		excitor_ = std::make_shared<KcLaplaceExcitor>();
		break;

	case kPrivate::k_rayleigh:
		excitor_ = std::make_shared<KcRayleighExcitor>();
		break;

	case kPrivate::k_lognorm:
		excitor_ = std::make_shared<KcLognormExcitor>();
		break;

	case kPrivate::k_cauchy:
		excitor_ = std::make_shared<KcCauchyExcitor>();
		break;

	case kPrivate::k_weibull:
		excitor_ = std::make_shared<KcWeibullExcitor>();
		break;

	case kPrivate::k_gauss:
	default:
		break;
	}

	excitor_ = std::make_shared<KcLaplaceExcitor>();
}


void KcPvExcitationSource::emitData_()
{
	auto data = std::make_shared<KcSampled1d>();
	data->reset(0, 0, step_);
	data->resize(size(0), 1);
	kReal* buf = data->data();
	for(kIndex i = 0; i < data->size(); i++)
	    *buf++ = excitor_->pulse();

	emit pushData(data);
}


void KcPvExcitationSource::timerEvent(QTimerEvent* event)
{
	emitData_();
}
