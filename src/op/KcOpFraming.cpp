#include "KcOpFraming.h"
#include <assert.h>
#include "KcSampled1d.h"
#include "KcSampled2d.h"
#include "imgui.h"


KcOpFraming::KcOpFraming() 
	: super_("Framing")
	, frameTime_(0.01)
    , shiftTime_(frameTime_ / 2)
{

}


int KcOpFraming::spec(kIndex outPort) const
{
	KpDataSpec sp(inputSpec_(outPort));
	if (sp.spec != 0)
	    ++sp.dim; // 比父数据多一个维度
	return sp.spec;
}


bool KcOpFraming::permitInput(int dataSpec, unsigned inPort) const
{
	KpDataSpec sp(dataSpec);
	return sp.dim == 1 && sp.type == k_sampled;
}


kIndex KcOpFraming::frameSize() const
{
	auto dx = step(0, 1);
	if (dx == 0)
		return 0;

	KtSampling<kReal> samp;
	samp.reset(0, frameTime_, dx, 0);
	return samp.size();
}


kIndex KcOpFraming::shiftSize() const
{
	auto dx = step(0, 1);
	if (dx == 0)
		return 0;

	KtSampling<kReal> samp;
	samp.reset(0, shiftTime_, dx, 0);
	return samp.size();
}


kRange KcOpFraming::range(kIndex outPort, kIndex axis) const
{
	assert(outPort == 0);

	if (axis == 0)
		return super_::range(outPort, 0);
	else if (axis == 1)
		return { 0, frameTime_ };

	return super_::range(outPort, axis - 1);
}


kReal KcOpFraming::step(kIndex outPort, kIndex axis) const
{
	if (axis == 0)
		return shiftTime_;

	return inputStep_(axis - 1);
}


kIndex KcOpFraming::size(kIndex outPort, kIndex axis) const
{
	if (axis == 1)
		return frameSize();
	else if (axis == 0) {
		// framing_成员可能不是最新状态，这里重构一个临时对象
		KtFraming<kReal> fram(frameSize(), channels(0), shiftSize());
		KtSampling<kReal> samp;
		auto r = range(0, 0);
		auto dx = step(0, 1);
		if (r.empty() || dx == 0)
			return 0;

		samp.reset(r.low(), r.high(), dx, 0);
		return fram.outFrames(samp.size(), false);
	}

	return super_::size(outPort, axis - 1);
}


bool KcOpFraming::onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins)
{
	assert(ins.size() == 1 && ins.front().first == 0);

	if (!super_::onStartPipeline(ins))
		return false;

	assert(inputs_.size() == 1 && inputs_.front() != nullptr);
	assert(odata_.size() == 1);

	auto in = ins.front().second;
	auto prov = std::dynamic_pointer_cast<KvDataProvider>(in->parent().lock());

	framing_ = std::make_unique<KtFraming<kReal>>(
		frameSize(), prov->channels(in->index()), shiftSize());

	odata_.front() = std::make_shared<KcSampled2d>(step(0, 0), step(0, 1), channels(0));
	return framing_ != nullptr;
}


void KcOpFraming::onStopPipeline()
{
	framing_.reset();
	// TODO: odata_.front() = nullptr; 

	super_::onStopPipeline();
}


void KcOpFraming::outputImpl_()
{
	assert(idata_.size() == 1 && idata_.front() != nullptr);
	assert(odata_.size() == 1);
	assert(framing_);

	assert(inputs_.size() == 1 && inputs_.front() != nullptr);
	auto in = inputs_.front();
	auto prov = std::dynamic_pointer_cast<KvDataProvider>(in->parent().lock());
	if (!prov->isStream(0)) // 输入是否stream
		framing_->reset(); // clear the buffer

	auto samp1d = std::dynamic_pointer_cast<KcSampled1d>(idata_.front());
	assert(samp1d);

	auto out = std::dynamic_pointer_cast<KcSampled2d>(odata_.front());
	auto frameNum = framing_->outFrames(samp1d->size(0), true);
	auto frameSize = this->frameSize();
	out->resize(frameNum, frameSize, framing_->channels());

	//kReal x0 = samp1d->sampling(0).low();
	//x0 -= framing_->buffered() * inputStep_();
	//out->reset(0, x0 + frameTime_ / 2, shiftTime_);
	//out->reset(1, x0, inputStep_());

	auto first = samp1d->data();
	auto last = first + samp1d->count();
	kIndex idx(0);
	framing_->apply(first, last, [&out, &idx](const kReal* data) {
		std::copy(data, data + out->size(1) * out->channels(), out->row(idx++));
		});

	// TODO: 使可配置
	if (!prov->isStream(0)) {
		framing_->flush([&out](const kReal* data) {
			out->pushBack(data, 1);
			});
	}

	odata_.front() = out;
}


void KcOpFraming::showPropertySet()
{
	KvDataOperator::showPropertySet();
	ImGui::Separator();

	ImGui::BeginDisabled(working_());

	const double frameTimeMin = super_::step(0, 0);
	if (ImGui::DragScalar("Frame Length(s)", ImGuiDataType_Double, &frameTime_,
		frameTime_ * 0.01, &frameTimeMin))
		setOutputExpired(0);

	ImGui::LabelText("Frame Size", "%d", frameSize());

	if (ImGui::DragScalar("Frame Shift(s)", ImGuiDataType_Double, &shiftTime_,
		shiftTime_ * 0.01, &frameTimeMin))
		setOutputExpired(0);

	ImGui::LabelText("Shift Size", "%d", shiftSize());

	ImGui::EndDisabled();
}
