#include "KcPvData.h"
#include "KvDiscreted.h"
#include "KvContinued.h"
#include "imgui.h"


KcPvData::KcPvData(const std::string_view& name, std::shared_ptr<KvData> data)
	: KvDataProvider(name), data_(data) 
{

}


kIndex KcPvData::dim(kIndex outPort) const
{
	return data_->dim();
}


kIndex KcPvData::channels(kIndex outPort) const
{
	return data_->channels();
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
