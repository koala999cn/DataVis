#include "KcImPlot2d.h"
#include "KcImPaint.h"
#include "plot/KcAxis.h"
#include "plot/KcCoord3d.h"
#include "KuStrUtil.h"


KcImPlot2d::KcImPlot2d(const std::string_view& name)
    : KvImWindow(name)
    , KvPlot2d(std::make_shared<KcImPaint>(camera_), std::make_shared<KcCoord3d>())
{
    minSize_[0] = 180, minSize_[1] = 180;
}


void KcImPlot2d::updateImpl_()
{
    // 设置窗口背景为plot的背景色
    ImGui::PushStyleColor(ImGuiCol_ChildBg, (const ImVec4&)bkgnd_.color);

    if (ImGui::BeginChild("##", ImVec2(0, 0), false, ImGuiWindowFlags_NoMove)) {

        // 更新摄像机的视图
        auto pos = ImGui::GetWindowPos();
        auto sz = ImGui::GetWindowSize();
        paint_->setViewport({ { pos.x + margins_.left(), pos.y + margins_.bottom() },
            { pos.x + sz.x - margins_.right(), pos.y + sz.y - margins_.top() } }
        );

        auto lower = coord().lower();
        auto upper = coord().upper();

        camera_.projMatrix() = KtMatrix4<float_t>::projectOrtho(lower, upper);

        // 绘制3d数据图
        KvPlot2d::update();
    }

    ImGui::EndChild();

    ImGui::PopStyleColor();
}


std::string KcImPlot2d::label() const
{
    // Using "###" to display a changing title but keep a static identifier
    return name() + "###" + KuStrUtil::toString(id());
}