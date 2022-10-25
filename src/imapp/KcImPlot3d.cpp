#include "KcImPlot3d.h"
#include "KcImPaint.h"
#include "imgui.h"
#include "plot/KcCoord2d.h"
#include "KuStrUtil.h"


KcImPlot3d::KcImPlot3d(const std::string_view& name)
    : KvPlot3d(std::make_shared<KcImPaint>(camera_), std::make_shared<KcCoord2d>())
    , KvImWindow(name)
    , trackball_(orient_)
{
    minSize_[0] = 240, minSize_[1] = 240;
}


void KcImPlot3d::updateImpl_()
{
    // ���ô��ڱ���Ϊplot�ı���ɫ
    ImGui::PushStyleColor(ImGuiCol_ChildBg, (const ImVec4&)bkgnd_.color); // TODO: check the style

    if (ImGui::BeginChild("##", ImVec2(0, 0), false, ImGuiWindowFlags_NoMove)) {

        // �������������ͼ
        auto pos = ImGui::GetWindowPos();
        auto sz = ImGui::GetWindowSize();
        paint_->setViewport({ { pos.x, pos.y }, { pos.x + sz.x, pos.y + sz.y } });

        // ��������¼�
        if (ImGui::IsWindowFocused() && ImGui::IsMouseHoveringRect(pos, { pos.x + sz.x, pos.y + sz.y }))
            handleMouseInput_();

        // ����3d����ͼ
        KvPlot3d::update();
    }

    ImGui::EndChild();

    ImGui::PopStyleColor();
}


void KcImPlot3d::handleMouseInput_()
{
    // �������drag�¼���ת��trackball�������������λ��
    if (ImGui::IsMouseClicked(0)) {
        auto mousePos = ImGui::GetMousePos();
        auto pivot = camera_.worldToScreen(coord().center());
        auto sz = ImGui::GetWindowSize();
        trackball_.reset({ mousePos.x, mousePos.y }, pivot, { sz.x / 2, sz.y / 2 });
    }
    if (ImGui::IsMouseDragging(0)) {
        auto d = ImGui::GetMouseDragDelta(0);
        trackball_.steer(d.x, d.y);
    }

    // �������wheel�¼���ʵ�����Ź���
    ImGuiIO& io = ImGui::GetIO();
    if (io.MouseWheel != 0) {

        auto factor = 1 + io.MouseWheel / 25;
        if (factor <= 0.1f)
            factor = 0.1f;
        else if (factor > 1.5f)
            factor = 1.5f;

        if (io.KeyCtrl) // ��ͬʱ����CTRL��ʱ������������ϵrange
            coord().zoom(factor);
        else // ��������plot
           zoom_ *= factor;
    }

    if (ImGui::IsMouseDragging(1)) {
        auto sz = ImGui::GetWindowSize();
        auto d = ImGui::GetMouseDragDelta(1);
        auto dx = d.x / sz.x;
        auto dy = -d.y / sz.y; // ��Ļ��y����������ͼ��y�����귴�򣬴˴�ȡ-d.y

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