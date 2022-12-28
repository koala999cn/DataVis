#include "KcRdPlot3d.h"
#include "imapp/KcImPlot3d.h"
#include "plot/KcGraph.h"
#include "plot/KcScatter.h"
#include "plot/KcBars3d.h"
#include "plot/KvCoord.h" // TODO: 此处不该引用KvCoord文件
#include "prov/KvDataProvider.h"
#include "KuStrUtil.h"
#include "imguix.h"


KcRdPlot3d::KcRdPlot3d()
	: super_("Plot3d", std::make_shared<KcImPlot3d>("Plot3d"))
{

}


std::vector<KvPlottable*> KcRdPlot3d::createPlottable_(KcPortNode* port)
{
	auto prov = std::dynamic_pointer_cast<KvDataProvider>(port->parent().lock());

	// 根据prov自动选择图类型
	if (prov->isScattered(port->index()) || prov->isSeries(port->index()))
	    return createPlts_<KcScatter>(port);
	else if (prov->isContinued(port->index()) || prov->isSampled(port->index()))
		return createPlts_<KcGraph>(port);
	else // TODO:
		return createPlts_<KcScatter>(port);
		
}


void KcRdPlot3d::showProperySet()
{
	super_::showProperySet();

	if (!ImGuiX::treePush("Projection", true))
		return;

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

	ImGuiX::treePop();
}


unsigned KcRdPlot3d::supportPlottableTypes_() const
{
	return 3;
}


int KcRdPlot3d::plottableType_(KvPlottable* plt) const
{
	if (dynamic_cast<KcGraph*>(plt))
		return 0;
	else if (dynamic_cast<KcScatter*>(plt))
		return 1;
	else if (dynamic_cast<KcBars3d*>(plt))
		return 2;

	return -1;
}


const char* KcRdPlot3d::plottableTypeStr_(int iType) const
{
	assert(iType < supportPlottableTypes_());

	static const char* pltTypes[] = {
		"graph", "scatter", "bar"
	};

	return pltTypes[iType];
}


KvPlottable* KcRdPlot3d::newPlottable_(int iType, const std::string& name)
{
	switch (iType)
	{
	case 0:
		return new KcGraph(name);

	case 1:
		return new KcScatter(name);

	case 2:
		return new KcBars3d(name);
	}

	return nullptr;
}
