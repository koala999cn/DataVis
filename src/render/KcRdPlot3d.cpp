#include "KcRdPlot3d.h"
#include "imapp/KcImPlot3d.h"
#include "plot/KcGraph.h"
#include "plot/KcScatter.h"
#include "plot/KcLineFilled.h"
#include "plot/KcBars3d.h"
#include "plot/KcSurface.h"
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

	ImGui::Checkbox("Isotropic", &plot3d->isotropic());
	
	double minVal(0.1), maxVal(10);
	static const char* format = "%.3f";
	ImGui::DragScalar("Zoom", ImGuiDataType_Double, &plot3d->zoom(), 0.01, &minVal, &maxVal, format);
	ImGui::DragScalarN("Scale", ImGuiDataType_Double, plot3d->scale(), 3, 0.01, &minVal, &maxVal, format);
	ImGui::DragScalarN("Shift", ImGuiDataType_Double, plot3d->shift(), 2, 1, 0, 0, "%.1 px");

	auto& orient = plot3d->orient();
	mat3d<> rot;
	orient.toRotateMatrix(rot);
	point3d angle;
	rot.toEulerAngleXYZ(angle);
	angle *= KtuMath<double>::rad2Deg(1);
	minVal = -180;
	maxVal = 180;
	if(ImGui::DragScalarN("Rotation", ImGuiDataType_Double, angle, 3, 0.5, &minVal, &maxVal, "%.1f deg")) {
		angle *= KtuMath<double>::deg2Rad(1);
		rot.fromEulerAngleXYZ(angle);
		orient = quatd(rot);
	}

	ImGuiX::treePop();
}


unsigned KcRdPlot3d::supportPlottableTypes_() const
{
	return 5;
}


int KcRdPlot3d::plottableType_(KvPlottable* plt) const
{
	if (dynamic_cast<KcGraph*>(plt))
		return 0;
	else if (dynamic_cast<KcScatter*>(plt))
		return 1;
	else if (dynamic_cast<KcLineFilled*>(plt))
		return 2;
	else if (dynamic_cast<KcBars3d*>(plt))
		return 3;
	else if (dynamic_cast<KcSurface*>(plt))
		return 4;

	return -1;
}


const char* KcRdPlot3d::plottableTypeStr_(int iType) const
{
	assert(iType < supportPlottableTypes_());

	static const char* pltTypes[] = {
		"graph", "scatter", "line-filled", "bar", "surface"
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
		return new KcLineFilled(name);

	case 3:
		return new KcBars3d(name);

	case 4:
		return new KcSurface(name);
	}

	return nullptr;
}


bool KcRdPlot3d::plottableMatchData_(int iType, const KvData& d) const
{
	switch (iType)
	{
	case 2:
		return d.dim() == 1;

	case 4:
		return d.dim() == 2;

	default:
		break;
	}

	return true;
}


namespace kPrivate
{
	void showPlottableSpecificProperty1d(KvPlottable* plt);
	void showPlottableSpecificProperty2d(KvPlottable* plt);
}

void KcRdPlot3d::showPlottableSpecificProperty_(unsigned idx)
{
	auto plt = plot_->plottableAt(idx);
	if (dynamic_cast<KcSurface*>(plt)) {
		kPrivate::showPlottableSpecificProperty2d(plt);
	}
	else {
		kPrivate::showPlottableSpecificProperty1d(plt);
	}
}
