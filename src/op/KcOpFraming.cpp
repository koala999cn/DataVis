#include "KcOpFraming.h"
#include <assert.h>
#include "KcSampled1d.h"
#include "KcSampled2d.h"
#include "imgui.h"
#include "imapp/KsImApp.h"
#include "imapp/KgPipeline.h"


KcOpFraming::KcOpFraming() 
	: KvDataOperator("Framing")
	, frameTime_(0.01)
    , shiftTime_(frameTime_ / 2)
{

}


int KcOpFraming::spec(kIndex outPort) const
{
	KpDataSpec sp(super_::spec(outPort));
	++sp.dim; // 比父数据多一个维度
	return sp.spec;
}


bool KcOpFraming::permitInput(int dataSpec, unsigned inPort) const
{
	KpDataSpec sp(dataSpec);
	return sp.dim == 1 && sp.type == k_sampled;
}


kReal KcOpFraming::inputStep_() const
{
	auto in = inputs_.front();
	if (in == nullptr)
		return 1.0 / 8000.;

	return super_::step(in->index(), 0);
}


kIndex KcOpFraming::frameSize() const
{
	KtSampling<kReal> samp;
	samp.reset(0, frameTime_, inputStep_(), 0);
	return samp.size();
}


kIndex KcOpFraming::shiftSize() const
{
	KtSampling<kReal> samp;
	samp.reset(0, shiftTime_, inputStep_(), 0);
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
	else if (axis == 0) {
		// framing_成员可能不是最新状态，这里重构一个临时对象
		KtFraming<kReal> fram(frameSize(), channels(0), shiftSize());
		KtSampling<kReal> samp;
		auto r = range(0, 0);
		samp.reset(r.low(), r.high(), inputStep_(), 0);
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
}


void KcOpFraming::output()
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


void KcOpFraming::showProperySet()
{
	KvDataOperator::showProperySet();
	ImGui::Separator();

	bool disable = KsImApp::singleton().pipeline().running();
	ImGui::BeginDisabled(disable);

	const double frameTimeMin = super_::step(0, 0);
	if (ImGui::DragScalar("Frame Length(s)", ImGuiDataType_Double, &frameTime_,
		frameTime_ * 0.01, &frameTimeMin))
		KsImApp::singleton().pipeline().notifyOutputChanged(this, 0);

	ImGui::LabelText("Frame Size", "%d", frameSize());

	if (ImGui::DragScalar("Frame Shift(s)", ImGuiDataType_Double, &shiftTime_,
		shiftTime_ * 0.01, &frameTimeMin))
		KsImApp::singleton().pipeline().notifyOutputChanged(this, 0);

	ImGui::LabelText("Shift Size", "%d", shiftSize());

	ImGui::EndDisabled();
}


bool KcOpFraming::onInputChanged(KcPortNode* outPort, unsigned inPort)
{
	assert(outPort == inputs_.front());
	
	// 所有属性都已动态化，此处不需要额外工作

	return true;
}
