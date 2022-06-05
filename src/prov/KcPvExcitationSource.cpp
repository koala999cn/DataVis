#include "KcPvExcitationSource.h"
#include "KtSampling.h"
#include "excitors.h"
#include "KcSampled1d.h"
#include <chrono>


KcPvExcitationSource::KcPvExcitationSource()
	: KvDataProvider("excitor")
{
	duration_ = 1024;
	step_ = 1;
	repeat_ = false;
	timerId_ = 0;
	excitor_ = std::make_shared<KcGaussExcitor>();
}


kRange KcPvExcitationSource::range(kIndex axis) const
{
	return axis == 0 ? kRange(0, duration_) : kRange(-1, 1);
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


bool KcPvExcitationSource::running() const
{
	return timerId_ != 0;
}


namespace kPrivate
{
	enum KeExcitationSourceProperty
	{
		k_duration,
		k_step,
		k_repeat,
	};
}


KcPvExcitationSource::kPropertySet KcPvExcitationSource::propertySet() const
{
	kPropertySet ps;
	KpProperty prop;

	prop.id = kPrivate::k_duration;
	prop.name = tr("Duration");
	prop.val = duration_;
	prop.minVal = kReal(0);
	prop.maxVal = std::numeric_limits<kReal>::max();
	prop.step = kReal(1);
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


bool KcPvExcitationSource::startImpl_()
{
	emitData_();

	if (repeat_) {
		timerId_ = startTimer(std::chrono::milliseconds(static_cast<long>(duration_ * 1000)));
		if (timerId_ == 0)
			return false;
	}

	return true;
}


bool KcPvExcitationSource::stopImpl_() 
{
	killTimer(timerId_);
	timerId_ = 0;

	return true;
}


void KcPvExcitationSource::setPropertyImpl_(int id, const QVariant& newVal)
{
	switch (id)
	{
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


void KcPvExcitationSource::emitData_()
{
	auto data = std::make_shared<KcSampled1d>();
	data->reset(0, 0, step_);
	data->resize(size(0), 1);
	kReal* buf = data->data();
	for(kIndex i = 0; i < data->size(); i++)
	    *buf++ = excitor_->pulse();

	emit onData(data);
}


void KcPvExcitationSource::timerEvent(QTimerEvent* event)
{
	emitData_();
}
