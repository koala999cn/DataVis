#include "KcImPlot3d.h"
#include "KcImOglPaint.h"
#include "imgui.h"
#include "plot/KcCoord3d.h"
#include "KuStrUtil.h"


KcImPlot3d::KcImPlot3d(const std::string_view& name)
    : KvPlot3d(std::make_shared<KcImOglPaint>(camera_), std::make_shared<KcCoord3d>())
    , KvImWindow(name)
    , trackball_(orient_)
{
    minSize_[0] = 240, minSize_[1] = 240;
    dynamic_ = true;
}


void KcImPlot3d::updateImpl_()
{
    // 设置窗口背景为plot的背景色
    ImGui::PushStyleColor(ImGuiCol_ChildBg, (const ImVec4&)background().color); // TODO: check the style

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
    }

    ImGui::EndChild();

    ImGui::PopStyleColor();
}


void KcImPlot3d::handleMouseInput_()
{
    // 处理鼠标drag事件，转动trackball，更新摄像机方位角
    if (ImGui::IsMouseClicked(0)) {
        KtVector4<float_t> pivot(coord().center(), 1);
        if (coord().axisInversed()) // NB: 由于此处paint的变换矩阵堆栈尚未构建（update没有调用），所以手动处理坐标轴交换，下同
            pivot = coord().axisSwapMatrix() * pivot;

        auto mousePos = ImGui::GetMousePos();
        auto sz = ImGui::GetWindowSize();
        trackball_.reset({ mousePos.x, mousePos.y }, { pivot.x(), pivot.y() }, { sz.x / 2, sz.y / 2 });
    }

    if (ImGui::IsMouseDragging(0)) {
        auto d = ImGui::GetMouseDragDelta(0);
        auto q = trackball_.steer(d.x, d.y);

        if (coord().axisInversed()) {
            float_t angle;
            KtVector3<float_t> v;
            q.toAngleAxis(angle, v);
            auto v4 = coord().axisSwapMatrix() * KtVector4<float_t>(v, 0);
            q = quat(angle, vec3(v4.x(), v4.y(), v4.z()));
        }

        orient_ = q * orient_;
        orient_.normalize();
    }

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

    if (ImGui::IsMouseDragging(1)) {
        auto sz = ImGui::GetWindowSize();
        auto d = ImGui::GetMouseDragDelta(1);
        auto dx = d.x / sz.x;
        auto dy = -d.y / sz.y; // 屏幕的y轴坐标与视图的y轴坐标反向，此处取-d.y

        auto box = coord().boundingBox();
        auto delta = box.size() * point3(dx, dy, 0);
        shift_ += delta * 0.1f;
    }
}


std::string KcImPlot3d::label() const
{
    // Using "###" to display a changing title but keep a static identifier
    return name() + "###" + KuStrUtil::toString(id());
}