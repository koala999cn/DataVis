#include "KcImPlot2d.h"
#include "KcImOglPaint.h"
#include "plot/KcAxis.h"
#include "plot/KcCoord2d.h"
#include "KuStrUtil.h"


KcImPlot2d::KcImPlot2d(const std::string_view& name)
    : KvImWindow(name)
    , KvPlot2d(std::make_shared<KcImOglPaint>(camera_), std::make_shared<KcCoord2d>())
{
    minSize_[0] = 180, minSize_[1] = 180;
    dynamic_ = true;
}


void KcImPlot2d::updateImpl_()
{
    // ���ô��ڱ���Ϊplot�ı���ɫ
    ImGui::PushStyleColor(ImGuiCol_ChildBg, (const ImVec4&)background().color);

    if (ImGui::BeginChild("##", ImVec2(0, 0), false, ImGuiWindowFlags_NoMove)) {

        // �������������ͼ
        auto pos = ImGui::GetWindowPos();
        auto sz = ImGui::GetWindowSize();
        paint().setViewport({ { pos.x, pos.y }, { pos.x + sz.x, pos.y + sz.y } });

        // ��������ͼ
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