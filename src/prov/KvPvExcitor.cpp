#include "KvPvExcitor.h"
#include "KtSampling.h"
#include "KcSampled1d.h"
#include "KvExcitor.h"
#include "imgui.h"


int KvPvExcitor::spec(kIndex outPort) const
{
	KpDataSpec sp;
	sp.dim = 1;
	sp.channels = 1;
	sp.type = k_array;
	sp.stream = true;
	sp.dynamic = true;

	return sp.spec;
}


kRange KvPvExcitor::range(kIndex outPort, kIndex axis) const
{
	if (axis == 0)
		return kRange(0, ticksPerFrame_ * step(outPort, 0));
		
	return excitor_ ? excitor_->range() : kRange{ 0, 0 };
}


kReal KvPvExcitor::step(kIndex outPort, kIndex axis) const
{
	return axis == 0 ? 1 : KvDiscreted::k_nonuniform_step;
}


kIndex KvPvExcitor::size(kIndex outPort, kIndex axis) const
{
	return ticksPerFrame_;
}


void KvPvExcitor::onNewFrame(int frameIdx)
{
	if (typeChanged_) {
		typeChanged_ = false;
		excitor_ .reset(createExcitor_(type_));
	}
}


bool KvPvExcitor::onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins)
{
	assert(ins.empty());

	excitor_.reset(createExcitor_(type_));

	auto data = std::make_shared<KcSampled1d>();
	data->reset(0, 0, step(0, 0));
	data->resize(size(0, 0), 1);
	data_ = data;

	return true;
}


void KvPvExcitor::onStopPipeline()
{
	excitor_.reset();
	// data_ = nullptr;
}


void KvPvExcitor::output()
{
	assert(excitor_ && data_);

	auto samp = std::dynamic_pointer_cast<KcSampled1d>(data_);
	assert(samp);
	samp->resize(ticksPerFrame_);
	kReal* buf = samp->data();
	for (int i = 0; i < ticksPerFrame_; i++)
		*buf++ = excitor_->tick();
}


std::shared_ptr<KvData> KvPvExcitor::fetchData(kIndex outPort) const
{
	assert(outPort == 0);
	return data_;
}


unsigned KvPvExcitor::dataStamp(kIndex outPort) const
{
	return currentFrameIndex_();
}


void KvPvExcitor::showProperySet()
{
	super_::showProperySet();
	ImGui::Separator();

	if (ImGui::BeginCombo("Type", typeStr_(type_))) {
		for (unsigned i = 0; i < typeCount_(); i++)
			if (ImGui::Selectable(typeStr_(i), i == type_)) {
				type_ = i;
				typeChanged_ = true;
			}
		ImGui::EndCombo();
	}

	if (ImGui::DragInt("Ticks Per Frame", &ticksPerFrame_, 1, 1e10)) {
		if (ticksPerFrame_ < 1)
			ticksPerFrame_ = 1;
	}
}
