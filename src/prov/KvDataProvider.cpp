#include "KvDataProvider.h"
#include <assert.h>
#include "imgui.h"
#include "imapp/KcImDataView.h"
#include "imapp/KsImApp.h"
#include "imapp/KgImWindowManager.h"
#include "imapp/KgPipeline.h"
#include "KvDiscreted.h"


KvDataProvider::~KvDataProvider()
{
	if (win_)
		KsImApp::singleton().windowManager().releaseDynamic(win_);

	win_ = nullptr;
}


bool KvDataProvider::isContinued() const
{
	return size(0) == KvData::k_inf_size;
}


bool KvDataProvider::isDiscreted() const 
{
	return size(0) != KvData::k_inf_size;
}


bool KvDataProvider::isScattered() const
{
	return isDiscreted() && step(0) == KvDiscreted::k_nonuniform_step;
}


bool KvDataProvider::isSeries() const {
	return isDiscreted() && step(0) == 1;
}

bool KvDataProvider::isSampled() const 
{
	return isDiscreted() && step(0) != KvDiscreted::k_nonuniform_step;
}


void KvDataProvider::onInput(KcPortNode*, unsigned)
{
	// 对于数据提供者，断言该方法不会被调用

	assert(false);
}


void KvDataProvider::showProperySet()
{
	KvBlockNode::showProperySet();

	// 类型
	ImGui::LabelText("Type", isSampled() ? "sampled" : isContinued() ? "continued" : "scattered");

	// 维度
	ImGui::LabelText("Dim", "%d", dim());

	// 通道数
	ImGui::LabelText("Channels", "%d", channels());

	// 数据数量
	if (isDiscreted()) {
		ImGui::LabelText("Size", "%d", size());
		// TODO: tooltip: "number of data points per-channel"
	}

	// 采样间隔
	if (isSampled()) {
		ImGui::LabelText("Step", "%g", step(0));
		ImGui::LabelText("SampleRate", "%g", 1.0 / step(0));
	}

	// key range
	ImGui::LabelText("Key Range", "%g - %g", range(0).low(), range(0).high());

	// value range
	ImGui::LabelText("Value Range", "%g - %g", range(dim()).low(), range(dim()).high());
}


void KvDataProvider::onDoubleClicked()
{
	if (win_) {
		win_->setVisible(true);
	}
	else {
		// TODO: a easy way to get the shared_ptr of THIS
		auto node = KsImApp::singleton().pipeline().getNode(id());
		auto prov = std::dynamic_pointer_cast<KvDataProvider>(node);
		win_ = KsImApp::singleton().windowManager().registerDynamic<KcImDataView>(prov);
	}
}
