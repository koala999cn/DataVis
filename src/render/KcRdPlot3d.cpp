#include "KcRdPlot3d.h"
#include "imapp/KcImPlot3d.h"
#include "imapp/KcImPlottable3d.h"
#include "prov/KvDataProvider.h"
#include "KuStrUtil.h"
#include "imgui.h"


KcRdPlot3d::KcRdPlot3d()
	: super_("Plot3d", std::make_shared<KcImPlot3d>("Plot3d"))
{

}


std::vector<KvPlottable*> KcRdPlot3d::createPlottable_(KvDataProvider* prov)
{
	return { new KcImPlottable3d(prov->name()) };
}


void KcRdPlot3d::showProperySet()
{
	super_::showProperySet();

	auto plot3d = std::dynamic_pointer_cast<KvPlot3d>(plot_);

	ImGui::Checkbox("Ortho", &plot3d->ortho());

	ImGui::Checkbox("Isometric", &plot3d->isometric());
	
	float zoom = plot3d->zoom();
	if (ImGui::DragFloat("Zoom", &zoom, 0.1, 0.2, 5))
		plot3d->zoom() = zoom;

	point3f pt(plot3d->scale());
	if (ImGui::DragFloat3("Scale", pt, 0.1, 0.2, 2))
		plot3d->scale() = point3d(pt);

	pt = point3f(plot3d->shift());
	if (ImGui::DragFloat3("Shift", pt))
		plot3d->shift() = point3d(pt);;

	auto& orient = plot3d->orient();
	mat3d<> rot;
	orient.toRotateMatrix(rot);
	point3d angle;
	rot.toEulerAngleXYZ(angle);
	point3f anglef(angle);
	if (ImGui::DragFloat3("Rotation", anglef)) {
		rot.fromEulerAngleXYZ(point3d(anglef));
		orient = quatd(rot);
	}
	
	auto lower = point3f(plot3d->coordSystem().lower());
	auto upper = point3f(plot3d->coordSystem().upper());
	auto speed = (upper - lower) * 0.1;
	if (ImGui::DragFloatRange2("X-Axis", &lower.x(), &upper.x(), speed.x()))
		plot3d->coordSystem().setExtents(lower, upper);
	if (ImGui::DragFloatRange2("Y-Axis", &lower.y(), &upper.y(), speed.y()))
		plot3d->coordSystem().setExtents(lower, upper);
	if (ImGui::DragFloatRange2("Z-Axis", &lower.z(), &upper.z(), speed.z()))
		plot3d->coordSystem().setExtents(lower, upper);
}
