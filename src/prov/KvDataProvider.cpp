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


bool KvDataProvider::isContinued(kIndex outPort) const
{
	return size(outPort, 0) == KvData::k_inf_size;
}


bool KvDataProvider::isDiscreted(kIndex outPort) const
{
	return size(outPort, 0) != KvData::k_inf_size;
}


bool KvDataProvider::isScattered(kIndex outPort) const
{
	return isDiscreted(outPort) && 
		step(outPort, 0) == KvDiscreted::k_nonuniform_step;
}


bool KvDataProvider::isSeries(kIndex outPort) const {
	return isDiscreted(outPort) && step(outPort, 0) == 1;
}

bool KvDataProvider::isSampled(kIndex outPort) const
{
	return isDiscreted(outPort) && 
		step(outPort, 0) != KvDiscreted::k_nonuniform_step;
}


void KvDataProvider::onInput(KcPortNode*, unsigned)
{
	// 对于数据提供者，断言该方法不会被调用

	assert(false);
}


void KvDataProvider::showProperySet()
{
	KvBlockNode::showProperySet();

	unsigned outPort = 0; // TODO: 多端口

	// 类型
	ImGui::LabelText("Type", isSampled(outPort) ? "sampled" : 
		isContinued(outPort) ? "continued" : "scattered");

	// 维度
	ImGui::LabelText("Dim", "%d", dim(outPort));

	// 通道数
	ImGui::LabelText("Channels", "%d", channels(outPort));

	// 数据数量
	if (isDiscreted(outPort)) {
		ImGui::LabelText("Size", "%d", size(outPort));
		// TODO: tooltip: "number of data points per-channel"
	}

	// 采样间隔
	if (isSampled(outPort)) {
		ImGui::LabelText("Step", "%g", step(outPort, 0));
		ImGui::LabelText("SampleRate", "%g", 1.0 / step(outPort, 0));
	}

	// TODO: 多维度
	// 
	// key range
	ImGui::LabelText("Key Range", "%g - %g", 
		range(outPort, 0).low(), range(outPort, 0).high());

	// value range
	ImGui::LabelText("Value Range", "%g - %g", 
		range(outPort, (outPort)).low(), range(outPort, dim(outPort)).high());
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
