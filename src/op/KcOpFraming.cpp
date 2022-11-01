#include "KcOpFraming.h"
#include <assert.h>
#include "KcSampled1d.h"
#include "KcSampled2d.h"
#include "imgui.h"
#include "imapp/KsImApp.h"
#include "imapp/KgPipeline.h"


KcOpFraming::KcOpFraming() 
	: KvDataOperator("Framing")
	, dx_(1)
	, frameTime_(512 * dx_)
    , shiftTime_(frameTime_ / 2)
{

}


int KcOpFraming::spec(kIndex outPort) const
{
	KpDataSpec sp(super_::spec(outPort));
	sp.type = k_sampled;
	sp.dim = 2; // 比父数据多一个维度
	sp.stream = true;
	sp.dynamic = true;
	return sp.spec;
}


bool KcOpFraming::permitInput(int dataSpec, unsigned inPort) const
{
	KpDataSpec sp(dataSpec);
	return sp.dim == 1 && sp.type == k_sampled;
}


kIndex KcOpFraming::frameSize() const
{
	KtSampling<kReal> samp;
	samp.reset(0, frameTime_, dx_, 0);
	return samp.size();
}


kIndex KcOpFraming::shiftSize() const
{
	KtSampling<kReal> samp;
	samp.reset(0, shiftTime_, dx_, 0);
	return samp.size();
}


kRange KcOpFraming::range(kIndex outPort, kIndex axis) const
{
	assert(outPort == 0);

	if (axis == 1)
		return { 0, frameTime_ };
	else if(axis == 0)
	    return super_::range(outPort, 0);
	
	return super_::range(outPort, axis - 1);
}


kReal KcOpFraming::step(kIndex outPort, kIndex axis) const
{
	if (axis == 0)
		return shiftTime_;

	return super_::step(outPort, axis - 1);
}


kIndex KcOpFraming::size(kIndex outPort, kIndex axis) const
{
	if (axis == 1)
		return frameSize();
	else if (axis == 0)
		return framing_ ? framing_->outFrames(super_::size(outPort, 0), false) : 0;

	return super_::size(outPort, axis - 1);
}


void KcOpFraming::makeFraming_()
{
	assert(inputs_.size() == 1 && inputs_.front() != nullptr);
	auto in = inputs_.front();
	auto prov = std::dynamic_pointer_cast<KvDataProvider>(in->parent().lock());
	if (!framing_ || dx_ != prov->step(in->index(), 0) ||
		framing_->channels() != prov->channels(in->index()) ||
		framing_->size() != frameSize() ||
		framing_->shift() != shiftSize()) {
		dx_ = prov->step(in->index(), 0);
		framing_ = std::make_unique<KtFraming<kReal>>(
			frameSize(), prov->channels(in->index()), shiftSize());
	}
}


bool KcOpFraming::onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins)
{
	if (!super_::onStartPipeline(ins))
		return false;

	makeFraming_();
	odata_.resize(1);
	odata_.front() = std::make_shared<KcSampled2d>(step(0, 0), step(0, 1), channels(0));
	return framing_ != nullptr;
}


void KcOpFraming::output()
{
	assert(idata_.size() == 1 && idata_.front() != nullptr);
	assert(odata_.size() == 1);
	assert(framing_);

	auto samp1d = std::dynamic_pointer_cast<KcSampled1d>(idata_.front());
	assert(samp1d && dx_ == samp1d->step(0));

	auto out = std::dynamic_pointer_cast<KcSampled2d>(odata_.front());
	auto frameNum = framing_->outFrames(samp1d->size(), true);
	auto frameSize = this->frameSize();
	kReal x0 = samp1d->sampling(0).low();
	x0 -= framing_->buffered() * dx_;
	out->resize(frameNum, frameSize, framing_->channels());
	out->reset(0, x0 + frameTime_ / 2, shiftTime_);
	out->reset(1, x0, dx_);

	auto first = samp1d->data();
	auto last = first + samp1d->size();
	kIndex idx(0);
	framing_->apply(first, last, [&out, &idx](const kReal* data) {
		std::copy(data, data + out->size(1), out->row(idx++));
		});
}


void KcOpFraming::showProperySet()
{
	KvDataOperator::showProperySet();
	ImGui::Separator();

	bool disable = KsImApp::singleton().pipeline().running();
	ImGui::BeginDisabled(disable);

	const double frameTimeMin = super_::step(0, 0);
	ImGui::DragScalar("Frame Length(s)", ImGuiDataType_Double, &frameTime_,
		frameTime_ * 0.01, &frameTimeMin);

	ImGui::DragScalar("Frame Shift(s)", ImGuiDataType_Double, &shiftTime_,
		shiftTime_ * 0.01, &frameTimeMin);

	ImGui::LabelText("Shift Size", "%d", shiftSize());

	ImGui::EndDisabled();
}
