#include "KcImPlot2d.h"
#include "implot/implot.h"
#include "KcImPaint.h"
#include "plot/KcAxis.h"


KcImPlot2d::KcImPlot2d(const std::string_view& name)
    : KvImWindow(name)
    , KvPlot2d(std::make_shared<KcImPaint>(camera_))
{
    minSize_[0] = 180, minSize_[1] = 180;
}


void KcImPlot2d::updateImpl_()
{
    // ���ô��ڱ���Ϊplot�ı���ɫ
    ImGui::PushStyleColor(ImGuiCol_ChildBg, (const ImVec4&)bkclr_);

    if (ImGui::BeginChild("##", ImVec2(0, 0), false, ImGuiWindowFlags_NoMove)) {

        // �������������ͼ
        auto pos = ImGui::GetWindowPos();
        auto sz = ImGui::GetWindowSize();
        paint_->setViewport({ { pos.x + margins_.left(), pos.y + margins_.bottom() },
            { pos.x + sz.x - margins_.right(), pos.y + sz.y - margins_.top() } }
        );

        auto lower = coord().lower();
        auto upper = coord().upper();

        camera_.projectOrtho(lower, upper, -1, 1);

        // ����3d����ͼ
        KvPlot2d::update();
    }

    ImGui::EndChild();

    ImGui::PopStyleColor();
}