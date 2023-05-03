#include "KcImPlot2d.h"
#include "KcImOglPaint.h"
#include "plot/KcAxis.h"
#include "plot/KcCoord2d.h"
#include "KuStrUtil.h"
#include "KuPlotContextMenu.h"


KcImPlot2d::KcImPlot2d(const std::string_view& name)
    : KvImWindow(name)
    , KvPlot2d(std::make_shared<KcImOglPaint>(), std::make_shared<KcCoord2d>())
{
    minSize_[0] = 180, minSize_[1] = 180;
    dynamic_ = true;
}


KtCamera<double>& KcImPlot2d::camera()
{
    return ((KvPaintHelper&)paint()).camera();
}


const KtCamera<double>& KcImPlot2d::camera() const
{
    return ((const KvPaintHelper&)paint()).camera();
}


void KcImPlot2d::updateImpl_()
{
    if (ImGui::BeginChild("##", ImVec2(0, 0), false, ImGuiWindowFlags_NoMove)) {

        // 更新摄像机的视图
        auto pos = ImGui::GetWindowPos();
        auto sz = ImGui::GetWindowSize();
        paint().setViewport({ { pos.x, pos.y }, { pos.x + sz.x, pos.y + sz.y } });

        // 绘制数据图
        KvPlot2d::update();

        // context menu
        KuPlotContextMenu::update(this);
    }

    ImGui::EndChild();
}


std::string KcImPlot2d::label() const
{
    // Using "###" to display a changing title but keep a static identifier
    return name() + "###" + KuStrUtil::toString(id());
}