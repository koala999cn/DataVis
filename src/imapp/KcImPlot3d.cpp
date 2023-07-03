#include "KcImPlot3d.h"
#include "KcPangoPaint.h"
#include "imgui.h"
#include "plot/KcCoord3d.h"
#include "plot/KvPaint.h"
#include "KuPlotContextMenu.h"


KcImPlot3d::KcImPlot3d(const std::string_view& name)
    : KvPlot3d(std::make_shared<KcPangoPaint>(), std::make_shared<KcCoord3d>())
    , KvImWindow(name)
{
    minSize_[0] = 240, minSize_[1] = 240;
    dynamic_ = true;
}


KtCamera<double>& KcImPlot3d::camera()
{
    return ((KvPaintHelper&)paint()).camera();
}


const KtCamera<double>& KcImPlot3d::camera() const
{
    return ((const KvPaintHelper&)paint()).camera();
}


void KcImPlot3d::updateImpl_()
{
    if (ImGui::BeginChild("##", ImVec2(0, 0), false, ImGuiWindowFlags_NoMove)) {

        // 更新摄像机的视图
        auto pos = ImGui::GetWindowPos();
        auto sz = ImGui::GetWindowSize();
        paint().setViewport({ { pos.x, pos.y }, { pos.x + sz.x, pos.y + sz.y } });

        // 处理鼠标事件
        if (ImGui::IsWindowFocused() && ImGui::IsMouseHoveringRect(pos, { pos.x + sz.x, pos.y + sz.y }))
            handleMouseInput_();

        // 绘制3d数据图
        KvPlot3d::update();

        // context menu
        KuPlotContextMenu::update(this);
    }

    ImGui::EndChild();
}


void KcImPlot3d::handleMouseInput_()
{
    // 处理鼠标wheel事件，实现缩放功能
    ImGuiIO& io = ImGui::GetIO();
    if (io.MouseWheel != 0) {

        auto factor = 1 + io.MouseWheel / 25;
        if (factor <= 0.1f)
            factor = 0.1f;
        else if (factor > 1.5f)
            factor = 1.5f;

        if (io.KeyCtrl) // 当同时按下CTRL键时，仅缩放坐标系range
            coord().zoom(factor);
        else // 否则缩放plot
            zoom_ *= factor;
    }

    // 处理鼠标drag事件，转动trackball，更新摄像机方位角
    if (ImGui::IsMouseClicked(0)) {
        // NB: 始终以窗口中心点为pivot，而非世界坐标系的中心点
        auto mousePos = ImGui::GetMousePos();
        auto winPos = ImGui::GetWindowPos();
        auto sz = ImGui::GetWindowSize();
        trackball_.reset({ mousePos.x, mousePos.y }, { winPos.x + sz.x / 2, winPos.y + sz.y / 2 }, { sz.x / 2, sz.y / 2 });
    }

    if (ImGui::IsMouseDragging(0)) {
        auto d = ImGui::GetMouseDragDelta(0);

        if (io.KeyShift) {
            constexpr float shiftSpeed = 0.1f;
            auto sz = ImGui::GetWindowSize();
            shift_ += { d.x * shiftSpeed, d.y * shiftSpeed, 0 }; // shift_为屏幕坐标，直接运算
        }
        else {          
            auto q = trackball_.steer(d.x, d.y);
            orient_ = q * orient_;
            orient_.normalize();
        }
    }
}


std::string KcImPlot3d::label() const
{
    // Using "###" to display a changing title but keep a static identifier
    return name() + "###" + std::to_string(id());
}