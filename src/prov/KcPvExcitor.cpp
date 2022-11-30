#include "KcPvExcitor.h"
#include "KtSampling.h"
#include "excitors.h"
#include "KcSampled1d.h"
#include "imgui.h"
#include "imapp/KsImApp.h"
#include "imapp/KgPipeline.h"


KcPvExcitor::KcPvExcitor()
	: KvDataProvider("Excitor")
{

}


int KcPvExcitor::spec(kIndex outPort) const
{
	KpDataSpec sp;
	sp.dim = 1;
	sp.channels = 1;
	sp.type = k_array;
	sp.stream = true;
	sp.dynamic = true;

	return sp.spec;
}


kRange KcPvExcitor::range(kIndex outPort, kIndex axis) const
{
	if (axis == 0)
		return kRange(0, plusePerFrame_);
		
	return excitor_ ? excitor_->range() : kRange{ 0, 0 };
}


kReal KcPvExcitor::step(kIndex outPort, kIndex axis) const
{
	return axis == 0 ? 1 : KvDiscreted::k_nonuniform_step;
}


kIndex KcPvExcitor::size(kIndex outPort, kIndex axis) const
{
	return plusePerFrame_;
}


bool KcPvExcitor::onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins)
{
	assert(ins.empty());

	createExcitor_();

	auto data = std::make_shared<KcSampled1d>();
	data->reset(0, 0, step(0, 0));
	data->resize(size(0, 0), 1);
	data_ = data;

	return true;
}


void KcPvExcitor::onStopPipeline()
{
	excitor_.reset();
	// data_ = nullptr;
}


void KcPvExcitor::output()
{
	assert(excitor_ && data_);

	auto samp = std::dynamic_pointer_cast<KcSampled1d>(data_);
	assert(samp);

	kReal* buf = samp->data();
	for (kIndex i = 0; i < samp->size(0); i++)
		*buf++ = excitor_->tick();
}


std::shared_ptr<KvData> KcPvExcitor::fetchData(kIndex outPort) const
{
	assert(outPort == 0);
	return data_;
}


namespace kPrivate
{
	enum KeExcitorType
	{
		k_gauss,
		k_exponent,
		k_laplace,
		k_rayleigh,
		k_lognorm,
		k_cauchy,
		k_weibull
	};
}


void KcPvExcitor::showProperySet()
{
	static const char* typeStr[] = {
		"Gauss",
		"Exponent",
		"Laplace",
		"Rayleigh",
		"Lognorm",
		"Cauchy",
		"Weibull"
	};

	super_::showProperySet();
	ImGui::Separator();

	bool disable = KsImApp::singleton().pipeline().running();
	ImGui::BeginDisabled(disable);

	if (ImGui::BeginCombo("Distributiom", typeStr[type_])) {
		for (unsigned i = 0; i < std::size(typeStr); i++)
			if (ImGui::Selectable(typeStr[i], i == type_)) {
				type_ = i;
				notifyChanged_();
			}
		ImGui::EndCombo();
	}


	ImGui::EndDisabled();
}


void KcPvExcitor::createExcitor_()
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

	excitor_ = std::make_shared<KcGaussExcitor>();
}
