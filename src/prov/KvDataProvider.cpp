#include "KvDataProvider.h"
#include <assert.h>
#include "imgui.h"


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
