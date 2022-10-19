#include "KcRdPlot3d.h"
#include "imapp/KcImPlot3d.h"
#include "plot/KcGraph.h"
#include "plot/KcScatter.h"
#include "prov/KvDataProvider.h"
#include "KuStrUtil.h"
#include "imgui.h"


KcRdPlot3d::KcRdPlot3d()
	: super_("Plot3d", std::make_shared<KcImPlot3d>("Plot3d"))
{

}


std::vector<KvPlottable*> KcRdPlot3d::createPlottable_(KvDataProvider* prov)
{
	return { new KcGraph(prov->name()) };
}


void KcRdPlot3d::showProperySet()
{
	super_::showProperySet();

	auto plot3d = std::dynamic_pointer_cast<KvPlot3d>(plot_);

	ImGui::Checkbox("Ortho", &plot3d->ortho());

	ImGui::Checkbox("Isometric", &plot3d->isometric());
	
	double minVal(0.1), maxVal(10);
	static const char* format = "%.3f";
	ImGui::DragScalar("Zoom", ImGuiDataType_Double, &plot3d->zoom(), 0.01, &minVal, &maxVal, format);
	ImGui::DragScalarN("Scale", ImGuiDataType_Double, plot3d->scale(), 3, 0.01, &minVal, &maxVal, format);
	ImGui::DragScalarN("Shift", ImGuiDataType_Double, plot3d->shift(), 3, 
		plot_->coord().boundingBox().width() * 0.01, 0, 0, format);

	auto& orient = plot3d->orient();
	mat3d<> rot;
	orient.toRotateMatrix(rot);
	point3d angle;
	rot.toEulerAngleXYZ(angle);
	angle *= 180 / KtuMath<double>::pi;
	minVal = -180;
	maxVal = 180;
	if(ImGui::DragScalarN("Rotation", ImGuiDataType_Double, angle, 3, 0.5, &minVal, &maxVal, "%.1fdeg")) {
		angle *= KtuMath<double>::pi / 180;
		rot.fromEulerAngleXYZ(angle);
		orient = quatd(rot);
	}
	
	auto lower = point3f(plot3d->coord().lower());
	auto upper = point3f(plot3d->coord().upper());
	auto speed = (upper - lower) * 0.1;
	for (unsigned i = 0; i < speed.size(); i++)
		if (speed.at(i) == 0)
			speed.at(i) = 1;

	bool extendsChanged(false);
	if (ImGui::DragFloatRange2("X-Axis", &lower.x(), &upper.x(), speed.x()))
		extendsChanged = true;
	if (ImGui::DragFloatRange2("Y-Axis", &lower.y(), &upper.y(), speed.y())) 
		extendsChanged = true;
	if (ImGui::DragFloatRange2("Z-Axis", &lower.z(), &upper.z(), speed.z())) 
		extendsChanged = true;

	if (extendsChanged) {
		plot3d->coord().setExtents(lower, upper);
		plot3d->autoFit() = false;
	}
}
