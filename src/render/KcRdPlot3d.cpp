#include "KcRdPlot3d.h"
#include "imapp/KcImPlot3d.h"
#include "prov/KvDataProvider.h"
#include "KuStrUtil.h"
#include "imgui.h"


KcRdPlot3d::KcRdPlot3d()
	: super_("Plot3d", std::make_shared<KcImPlot3d>("Plot3d"))
{

}


std::vector<KvPlottable*> KcRdPlot3d::createPlottable_(KvDataProvider* prov)
{
	return {};
}


void KcRdPlot3d::showProperySet()
{
	super_::showProperySet();

	auto plot3d = std::dynamic_pointer_cast<KvPlot3d>(plot_);

	ImGui::Checkbox("Ortho", &plot3d->ortho());

	ImGui::Checkbox("Isometric", &plot3d->isometric());
	
	ImGui::DragFloat("Zoom", &plot3d->zoom(), 0.1, 0.2, 5);

	ImGui::DragFloat3("Scale", &plot3d->scale().x(), 0.1, 0.2, 2);

	ImGui::DragFloat3("Shift", &plot3d->shift().x());

	auto& orient = plot3d->orient();
	mat3f<> rot;
	orient.toRotateMatrix(rot);
	point3f angle;
	rot.toEulerAngleXYZ(angle);
	if (ImGui::DragFloat3("Rotation", &angle.x())) {
		rot.fromEulerAngleXYZ(angle);
		orient = quatf(rot);
	}
	
	auto lower = plot3d->coordSystem().lower();
	auto upper = plot3d->coordSystem().upper();
	if (ImGui::DragFloatRange2("X-Axis", &lower.x(), &upper.x(), (upper.x() - lower.x()) * 0.1))
		plot3d->coordSystem().setExtents(lower, upper);
	if (ImGui::DragFloatRange2("Y-Axis", &lower.y(), &upper.y(), (upper.y() - lower.y()) * 0.1))
		plot3d->coordSystem().setExtents(lower, upper);
	if (ImGui::DragFloatRange2("Z-Axis", &lower.z(), &upper.z(), (upper.z() - lower.z()) * 0.1))
		plot3d->coordSystem().setExtents(lower, upper);
}
