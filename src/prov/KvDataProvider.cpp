﻿#include "KvDataProvider.h"
#include <assert.h>
#include <string>
#include "imguix.h"
#include "imapp/KcImDataView.h"
#include "imapp/KsImApp.h"
#include "imapp/KgImWindowManager.h"
#include "imapp/KgPipeline.h"
#include "KvDiscreted.h"


KvDataProvider::~KvDataProvider()
{
	auto& winMgr = KsImApp::singleton().windowManager();
	auto win = winMgr.getWindow(windowId_);
	if (win != nullptr)
		winMgr.releaseWindow(windowId_);
}


bool KvDataProvider::isStream(kIndex outPort) const
{
	return KpDataSpec(spec(outPort)).stream;
}


bool KvDataProvider::isDynamic(kIndex outPort) const
{
	return KpDataSpec(spec(outPort)).dynamic;
}


kIndex KvDataProvider::dim(kIndex outPort) const
{
	return KpDataSpec(spec(outPort)).dim;
}


kIndex KvDataProvider::channels(kIndex outPort) const
{
	return KpDataSpec(spec(outPort)).channels;
}


kIndex KvDataProvider::total(kIndex outPort) const
{
	if (!isScattered(outPort)) {
		kIndex c(1);
		for (kIndex i = 0; i < dim(outPort); i++)
			c *= size(outPort, i);
		return c;
	}
	else
		return size(outPort, 0);
}


bool KvDataProvider::isContinued(kIndex outPort) const
{
	return KpDataSpec(spec(outPort)).type == k_continued;
}


bool KvDataProvider::isDiscreted(kIndex outPort) const
{
	return !isContinued(outPort);
}


bool KvDataProvider::isScattered(kIndex outPort) const
{
	return KpDataSpec(spec(outPort)).type == k_scattered;
}


bool KvDataProvider::isSeries(kIndex outPort) const 
{
	auto sp = KpDataSpec(spec(outPort));
	return sp.type == k_array && sp.dim == 1;
}


bool KvDataProvider::isSampled(kIndex outPort) const
{
	return KpDataSpec(spec(outPort)).type == k_sampled;
}


bool KvDataProvider::isArray(kIndex outPort) const
{
	return KpDataSpec(spec(outPort)).type == k_array;
}


std::string KvDataProvider::dataTypeStr(int spec)
{
	KpDataSpec sp(spec);

	switch (sp.type)
	{
	case k_sampled:
		return "sampled";

	case k_scattered:
		return "scattered";

	case k_continued:
		return "continued";

	case k_array:
		return sp.dim == 1 ? "series" : sp.dim == 2 ? "matrix" : "multi-array";

	case k_unknown:
		break;

	default:
		assert(false);
		break;
	}

	return "unknown";
}


void KvDataProvider::onInput(KcPortNode*, unsigned)
{
	// 对于数据提供者，断言该方法不会被调用
	assert(false);
}


void KvDataProvider::showPropertySet()
{
	super_::showPropertySet();

	unsigned outPort = 0; // TODO: 多端口
	KpDataSpec sp(spec(outPort));

	// 类型
	ImGui::LabelText("Type", dataTypeStr(sp.spec).c_str());

	// 维度
	ImGui::LabelText("Dim", "%d", sp.dim);

	// 通道数
	ImGui::LabelText("Channels", "%d", sp.channels);

	ImGui::LabelText("Streaming", sp.stream ? "true" : "false");

	ImGui::LabelText("Dynamic", sp.dynamic ? "true" : "false");

	ImGui::LabelText("Time Stamp", "%d", dataStamp(outPort));

	// 数据数量
	if (isDiscreted(outPort)) {
		if (dim(outPort) <= 1) {
			ImGui::LabelText("Size", "%d", size(outPort, 0));
		}
		else {
			std::string label = "Size(total = ";
			label += std::to_string(total(outPort));
			label += ")";
			if (ImGuiX::treePush(label.c_str(), true)) {
				for (kIndex i = 0; i < dim(outPort); i++) {
					std::string label("Dim");
					label += std::to_string(i + 1);
					ImGui::LabelText(label.c_str(), "%d", size(outPort, i));
				}
				ImGuiX::treePop();
			}
		}
	}

	// 采样间隔
	if (isSampled(outPort)) {
		if (dim(outPort) <= 1) {
			ImGui::LabelText("Step", "%g", step(outPort, 0));
			ImGui::LabelText("Frequency", "%g", 1.0 / step(outPort, 0));
		}
		else if (ImGuiX::treePush("Step", true)) {
			for (kIndex i = 0; i < dim(outPort); i++) {
				std::string label("Dim");
				label += std::to_string(i + 1);
				ImGui::LabelText(label.c_str(), "%g", step(outPort, i));
			}
			ImGuiX::treePop();
		}
	}

	if (dim(outPort) <= 1) {
		ImGui::LabelText("Key Range", "%g - %g",
			range(outPort, 0).low(), range(outPort, 0).high());
		ImGui::LabelText("Value Range", "%g - %g",
			range(outPort, dim(outPort)).low(), range(outPort, dim(outPort)).high());
	}
	else if (ImGuiX::treePush("Range", true)) {
		for (kIndex i = 0; i <= dim(outPort); i++) {
			std::string label("Dim");
			label += std::to_string(i + 1);
			ImGui::LabelText(label.c_str(), "%g - %g", 
				range(outPort, i).low(), range(outPort, i).high());
		}
		ImGuiX::treePop();
	}
}


void KvDataProvider::onDoubleClicked()
{
	auto& winMgr = KsImApp::singleton().windowManager();
	auto win = winMgr.getWindow(windowId_);
	if (win == nullptr) {
		// TODO: a easy way to get the shared_ptr of THIS
		auto node = KsImApp::singleton().pipeline().getNode(id());
		auto prov = std::dynamic_pointer_cast<KvDataProvider>(node);
		KsImApp::singleton().windowManager().registerWindow<KcImDataView>(prov);
	}
	else {
		// TODO: bring the window to front
	}
}


void KvDataProvider::notifyChanged(unsigned outPort)
{
	if (!working_()) {
		if (outPort == -1) {
			for (unsigned i = 0; i < outPorts(); i++)
				KsImApp::singleton().pipeline().notifyOutputChanged(this, i);
		}
		else {
			KsImApp::singleton().pipeline().notifyOutputChanged(this, outPort);
		}
	}
}


bool KvDataProvider::working_() const
{
	return KsImApp::singleton().pipeline().running();
}


unsigned KvDataProvider::currentFrameIndex_() const
{
	return KsImApp::singleton().pipeline().frameIndex();
}