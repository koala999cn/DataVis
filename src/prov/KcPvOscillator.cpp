#include "KcPvOscillator.h"
#include "KuOscillatorFactory.h"
#include "KvOscillator.h"
#include "KvDiscreted.h" // for k_nonuniform_step
#include "imgui.h"
#include "imapp/KsImApp.h"
#include "imapp/KgPipeline.h"


KcPvOscillator::KcPvOscillator()
	: super_("Oscillator")
{

}


int KcPvOscillator::spec(kIndex outPort) const
{
	KpDataSpec ds(super_::spec(outPort));
	ds.type = k_sampled; // Õñµ´Æ÷²úÉúsampledÐÅºÅ
	return ds.spec;
}


kReal KcPvOscillator::step(kIndex outPort, kIndex axis) const
{
	return axis == 0 ? 1 / tickRate_ : KvDiscreted::k_nonuniform_step;
}


unsigned KcPvOscillator::typeCount_() const
{
	return KuOscillatorFactory::typeCount();
}


const char* KcPvOscillator::typeStr_(int type) const
{
	return KuOscillatorFactory::typeName(type);
}


KvExcitor* KcPvOscillator::createExcitor_(int type)
{
	auto osc = KuOscillatorFactory::create(type);
	osc->reset(phase_ * osc->period());
	osc->setFreq(freq_);
	osc->setTickRate(tickRate_);
	return osc;
}


void KcPvOscillator::showProperySet()
{
	super_::showProperySet();

	bool disable = KsImApp::singleton().pipeline().running();
	ImGui::BeginDisabled(disable);
	ImGui::SliderFloat("Phase", &phase_, 0, 1);
	ImGui::EndDisabled();

	auto osc = std::dynamic_pointer_cast<KvOscillator>(excitor());

	if (ImGui::DragFloat("Frequency", &freq_, 1, 1e-10, 1e10)) {
		if (freq_ <= 0) freq_ = 1e-10;
		if (osc) osc->setFreq(freq_);
	}

	if (ImGui::DragFloat("Tick Rate", &tickRate_, 1, 1e-10, 1e10)) {
		if (tickRate_ <= 0) tickRate_ = 1e-10;
		if (osc) osc->setTickRate(tickRate_);
	}
}
