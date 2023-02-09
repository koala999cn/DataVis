#include "KcPvData.h"
#include "KvDiscreted.h"
#include "KvContinued.h"
#include "imapp/imguix.h"
#include "imapp/KsImApp.h"
#include "imapp/KgPipeline.h"


KcPvData::KcPvData(const std::string_view& name, std::shared_ptr<KvData> data)
	: KvDataProvider(name), data_(data) 
{
	notifyChanged(true ,true);
}


bool KcPvData::onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>&)
{
	dataStamp_ = currentFrameIndex_();
	return true;
}


void KcPvData::updateSpec_()
{
	KpDataSpec sp;

	if (data_) {

		sp.dim = data_->dim();
		sp.channels = data_->channels();

		if (data_->size() == data_->k_inf_size)
			sp.type = k_continued;
		else {
			auto disc = std::dynamic_pointer_cast<KvDiscreted>(data_);
			if (disc->step(0) == disc->k_nonuniform_step)
				sp.type = k_scattered;
			else {
				sp.type = k_unknown;
				for (unsigned i = 0; i < sp.dim; i++)
					if (disc->step(i) != 1) {
						sp.type = k_sampled;
						break;
					}
				if (sp.type == k_unknown)
					sp.type = k_array;
			}
		}
	}

	spec_ = sp.spec;
}


int KcPvData::spec(kIndex outPort) const
{
	return spec_;
}


kRange KcPvData::range(kIndex outPort, kIndex axis) const
{
	return axis == dim(outPort) ? valueRange_ : data_->range(axis);
}


kReal KcPvData::step(kIndex outPort, kIndex axis) const
{
	auto dis = std::dynamic_pointer_cast<KvDiscreted>(data_);
	return dis ? dis->step(axis) : 0;
}


kIndex KcPvData::size(kIndex outPort, kIndex axis) const
{
	auto dis = std::dynamic_pointer_cast<KvDiscreted>(data_);
	return dis ? dis->size(axis) : KvData::k_inf_size;
}


std::shared_ptr<KvData> KcPvData::fetchData(kIndex outPort) const
{
	assert(outPort == 0);
	return data_;
}


unsigned KcPvData::dataStamp(kIndex outPort) const
{ 
	assert(outPort == 0);
	return dataStamp_; 
}


void KcPvData::showPropertySet()
{
	super_::showPropertySet();
	
	if (data_ && data_->isContinued()) {
		auto cont = std::dynamic_pointer_cast<KvContinued>(data_);

		ImGui::Separator();

		if (ImGuiX::treePush("Range", true)) {
			char label[] = { 'X', '\0' };
			for (unsigned i = 0; i < data_->dim(); i++) {
				auto r = data_->range(i);
				float low = r.low(), high = r.high();
				if (ImGui::DragFloatRange2(label, &low, &high)) {
					cont->setRange(i, low, high);
					notifyChanged(false, true);
				}

				label[0] += 1;
			}

			ImGuiX::treePop();
		}
	}
}


void KcPvData::setData(const std::shared_ptr<KvData>& d)
{
	data_ = d;
	notifyChanged(true, true);
}


void KcPvData::notifyChanged(bool specChanged, bool dataChanged)
{
	if (specChanged)
	    updateSpec_();

	// if (dataChanged)
	    valueRange_ = data_ ? data_->valueRange() : kRange(0, 0);

	dataStamp_ = currentFrameIndex_();

	// TODO: 待删除 KsImApp::singleton().pipeline().notifyOutputChanged(this, 0);
}
