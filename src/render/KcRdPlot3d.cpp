#include "KcRdPlot3d.h"
#include "imapp/KcImPlot3d.h"
#include "imapp/KsImApp.h"
#include "imapp/KgImWindowManager.h"
#include "imapp/KcImPlottable.h"
#include "prov/KvDataProvider.h"
#include "dsp/KcDataMono.h"
#include "KuStrUtil.h"
#include "imgui.h"


KcRdPlot3d::KcRdPlot3d()
	: KvDataRender("plot3d")
{
	plot3d_ = std::make_shared<KcImPlot3d>("Plot3d");
	plot3d_->setVisible(false);
	KsImApp::singleton().windowManager().registerDynamic(plot3d_);
}


KcRdPlot3d::~KcRdPlot3d()
{
	KsImApp::singleton().windowManager().releaseDynamic(plot3d_);
	plot3d_ = nullptr;
}


void KcRdPlot3d::onInput(KcPortNode* outPort, unsigned inPort)
{
	assert(inPort == 0);

	auto pnode = outPort->parent().lock();
	assert(pnode);

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(pnode);
	assert(prov);

	auto r = port2Plts_.equal_range(outPort->id());
	assert(r.first != r.second);

	auto data = prov->fetchData(outPort->index());
	assert(data->channels() == std::distance(r.first, r.second));

	if (data->channels() == 1)
		r.first->second->data() = data;
	else {
		kIndex ch(0);
		auto disc = std::dynamic_pointer_cast<KvDiscreted>(data);
		assert(disc);

		for (auto i = r.first; i != r.second; i++) {
			assert(i->first == outPort->id());
			i->second->data() = std::make_shared<KcDataMono>(disc, ch++);
		}
	}
}


bool KcRdPlot3d::onNewLink(KcPortNode* from, KcPortNode* to)
{
	assert(to->parent().lock().get() == this && to->index() == 0);

	auto pnode = from->parent().lock();
	assert(pnode);

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(pnode);
	assert(prov);

	if (prov->channels() == 1) {
		auto plt = new KcImPlottable(pnode->name());
		plot3d_->addPlottable(plt);
		port2Plts_.insert(std::make_pair(from->id(), plt));
	}
	else {
		for (kIndex ch = 0; ch < prov->channels(); ch++) {
			std::string name = pnode->name() + " - ch" + KuStrUtil::toString(ch);
			auto plt = new KcImPlottable(name);
			plot3d_->addPlottable(plt);
			port2Plts_.insert(std::make_pair(from->id(), plt));
		}
	}

	return true;
}


void KcRdPlot3d::onDelLink(KcPortNode* from, KcPortNode* to)
{
	assert(to->parent().lock().get() == this && to->index() == 0);

	auto pnode = from->parent().lock();
	assert(pnode);

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(pnode);
	assert(prov);

	auto r = port2Plts_.equal_range(from->id());
	assert(r.first != r.second);

	for (auto i = r.first; i != r.second; i++)
		plot3d_->removePlottable(i->second);

	port2Plts_.erase(from->id());
}


bool KcRdPlot3d::onStartPipeline()
{
	plot3d_->setVisible(true);
	return true;
}


void KcRdPlot3d::showProperySet()
{
	super_::showProperySet();

	ImGui::ColorEdit4("Background", (float*)&plot3d_->background().r());

	ImGui::Checkbox("Ortho", &plot3d_->ortho());

	ImGui::Checkbox("Isometric", &plot3d_->isometric());
	
	ImGui::DragFloat("Zoom", &plot3d_->zoom(), 0.1, 0.2, 5);

	ImGui::DragFloat3("Scale", &plot3d_->scale().x(), 0.1, 0.2, 2);

	ImGui::DragFloat3("Shift", &plot3d_->shift().x());

	auto& orient = plot3d_->orient();
	mat3f<> rot;
	orient.toRotateMatrix(rot);
	point3f angle;
	rot.toEulerAngleXYZ(angle);
	if (ImGui::DragFloat3("Rotation", &angle.x())) {
		rot.fromEulerAngleXYZ(angle);
		orient = quatf(rot);
	}
	
	auto lower = plot3d_->coordSystem().lower();
	auto upper = plot3d_->coordSystem().upper();
	if (ImGui::DragFloatRange2("X-Axis", &lower.x(), &upper.x(), (upper.x() - lower.x()) * 0.1))
		plot3d_->coordSystem().setExtents(lower, upper);
	if (ImGui::DragFloatRange2("Y-Axis", &lower.y(), &upper.y(), (upper.y() - lower.y()) * 0.1))
		plot3d_->coordSystem().setExtents(lower, upper);
	if (ImGui::DragFloatRange2("Z-Axis", &lower.z(), &upper.z(), (upper.z() - lower.z()) * 0.1))
		plot3d_->coordSystem().setExtents(lower, upper);
}
