#include "KcPvData.h"
#include "KvDiscreted.h"
#include "KvContinued.h"
#include "imgui.h"


KcPvData::KcPvData(const std::string_view& name, std::shared_ptr<KvData> data)
	: KvDataProvider(name), data_(data) 
{
	updateSpec_();
}


void KcPvData::updateSpec_()
{
	if (data_ == nullptr)
		spec_ = 0;
	else {
		KpDataSpec sp;
		sp.stream = false;
		sp.dynamic = false;
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
				if (sp.type != k_unknown)
					sp.type = k_array;
			}
		}

		spec_ = sp.spec;
	}
}


int KcPvData::spec(kIndex outPort) const
{
	return spec_;
}


kRange KcPvData::range(kIndex outPort, kIndex axis) const
{
	return data_->range(axis);
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


void KcPvData::showProperySet()
{
	super_::showProperySet();
	
	if (data_->isContinued()) {
		auto cont = std::dynamic_pointer_cast<KvContinued>(data_);

		ImGui::Separator();

		if (ImGui::TreeNodeEx("Range", ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_DefaultOpen)) {
			char label[] = { 'x', '\0'};
			for (unsigned i = 0; i < data_->dim(); i++) {
				auto r = data_->range(i);
				float low = r.low(), high = r.high();
				if (ImGui::DragFloatRange2(label, &low, &high))
					cont->setRange(i, low, high);
				label[0] += 1;
			}

			ImGui::TreePop();
		}
	}
}
