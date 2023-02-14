#include "KcOpFraming.h"
#include <assert.h>
#include "KcSampled1d.h"
#include "KcSampled2d.h"
#include "dsp/KuDataUtil.h"
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


void KcOpFraming::setFrameSize(kIndex s)
{
	auto dx = step(0, 1);
	if (dx == 0)
		return;

	KtSampling<kReal> samp;
	samp.resetn(s, dx, 0);
	frameTime_ = samp.length();
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


void KcOpFraming::setShiftSize(kIndex s)
{
	auto dx = step(0, 1);
	if (dx == 0)
		return;

	KtSampling<kReal> samp;
	samp.resetn(s, dx, 0);
	shiftTime_ = samp.length();
}


kRange KcOpFraming::range(kIndex outPort, kIndex axis) const
{
	assert(outPort == 0);

	if (axis == 0)
		return inputRange_(outPort, 0);
	else if (axis == 1)
		return { 0, frameTime_ };

	return inputRange_(outPort, axis - 1);
}


kReal KcOpFraming::step(kIndex outPort, kIndex axis) const
{
	return axis == 0 ? shiftTime_ : inputStep_(axis - 1);
}


kIndex KcOpFraming::size(kIndex outPort, kIndex axis) const
{
	if (axis == 1)
		return frameSize();
	else if (axis == 0) {
		if (framing_)
			return framing_->outFrames(inputSize_(0), false);
		
		auto shiftS = shiftSize();
		return shiftS == 0 ? 0
			: KtFraming<kReal>(frameSize(), channels(0), shiftS).outFrames(inputSize_(0), false);
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
	return framing_ != nullptr && odata_.front() != nullptr;
}


void KcOpFraming::onStopPipeline()
{
	framing_.reset();
	super_::onStopPipeline();
}


void KcOpFraming::outputImpl_()
{
	assert(idata_.size() == 1 && idata_.front() != nullptr);
	assert(odata_.size() == 1);
	assert(framing_ && framing_->channels() == channels(0) 
		&& framing_->size() == frameSize() && framing_->shift() == shiftSize());
	
	if (!isStream(0))
		framing_->reset(); // 静态数据，清空framing的缓存

	auto g = KuDataUtil::valueGetter1d(idata_.front());
	auto buf = g.data;
	std::vector<kReal> vec;
	if (!buf) {
		vec = g.fetch(0, g.samples);
		buf = vec.data();
	}

	auto out = std::dynamic_pointer_cast<KcSampled2d>(odata_.front());
	auto frameNum = framing_->outFrames(g.samples, true);
	out->resize(frameNum, framing_->size(), framing_->channels());

	//kReal x0 = samp1d->sampling(0).low();
	//x0 -= framing_->buffered() * inputStep_();
	//out->reset(0, x0 + frameTime_ / 2, shiftTime_);
	//out->reset(1, x0, inputStep_());

	auto first = buf;
	auto last = first + g.samples;
	kIndex idx(0);
	framing_->apply(first, last, [&out, &idx](const kReal* data) {
		std::copy(data, data + out->stride(0), out->row(idx++));
		});

	// TODO: 使可配置
	if (!isStream(0)) {
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

	const float frameTimeMin = super_::step(0, 0);
	float t = frameTime_;
	if (ImGui::DragFloat("Frame Duration", &frameTime_, frameTimeMin, frameTimeMin, 
		super_::range(0, 0).length(), "%g sec")) {
		if (frameTime_ < frameTimeMin) 
			frameTime_ = frameTimeMin;
		setOutputExpired(0);
	}

	int s = frameSize();
	if (ImGui::DragInt("Frame Size", &s, 1, 0, 1e10) && s > 0) {
		setFrameSize(s);
		setOutputExpired(0);
	}

	if (ImGui::DragFloat("Shift Time", &shiftTime_, frameTimeMin, frameTimeMin,
		super_::range(0, 0).length(), "%g sec")) {
		if (shiftTime_ < frameTimeMin)
			shiftTime_ = frameTimeMin;
		setOutputExpired(0);
	}

	s = shiftSize();
	if (ImGui::DragInt("Shift Size", &s, 1, 0, 1e10) && s > 0) {
		setShiftSize(s);
		setOutputExpired(0);
	}
}


bool KcOpFraming::prepareOutput_()
{
	auto frameS = frameSize();
	auto shiftS = shiftSize();
	auto chs = channels(0);

	if (framing_->shift() != shiftS
		|| framing_->size() != frameS
		|| framing_->channels() != chs) {
		framing_ = std::make_unique<KtFraming<kReal>>(frameS, chs, shiftS);
		return true;
	}

	return false;
}
