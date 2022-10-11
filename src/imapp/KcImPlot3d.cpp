#include "KcImPlot3d.h"
#include "KcImPaint.h"
#include "imgui.h"


KcImPlot3d::KcImPlot3d(const std::string_view& name)
    : KvPlot3d(std::make_shared<KcImPaint>(camera_))
    , KvImWindow(name)
    , trackball_(orient_)
{
    minSize_[0] = 240, minSize_[1] = 240;
}


void KcImPlot3d::updateImpl_()
{
    // ���ô��ڱ���Ϊplot�ı���ɫ
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(bkclr_.r(), bkclr_.g(), bkclr_.b(), bkclr_.a()));

    if (ImGui::BeginChild("##", ImVec2(0, 0), false, ImGuiWindowFlags_NoMove)) {

        // �������������ͼ
        auto pos = ImGui::GetWindowPos();
        auto sz = ImGui::GetWindowSize();
        camera_.setViewport(pos.x, pos.y, sz.x, sz.y);

        // ��������¼�
        if (ImGui::IsMouseHoveringRect(pos, { pos.x + sz.x, pos.y + sz.y }))
            handleMouseInput_();

        // ����3d����ͼ
        KvPlot3d::update();
    }

    ImGui::EndChild();

    ImGui::PopStyleColor();
}


void KcImPlot3d::autoProject_()
{
    auto lower = coordSystem().lower();
    auto upper = coordSystem().upper();
    auto center = lower + (upper - lower) / 2;
    double radius = coordSystem().diag() / 2;

    auto zoom = zoom_;
    auto scale = scale_;
    auto shift = shift_;
    if (!isometric_) {
        zoom *= 2 * radius / sqrt(3.);
        auto factor = upper - lower;
        for (unsigned i = 0; i < 3; i++)
            if (factor.at(i) == 0)
                factor.at(i) = 1;
        scale /= factor;
    }
    scale *= zoom;

    // ��ƽ����AABB���ĵ㣬�����ţ�����ת��������û��趨��ƽ��
    camera_.viewMatrix() = mat4::buildTanslation(shift)
                         * mat4::buildRotation(orient_)
                         * mat4::buildScale(scale)
                         * mat4::buildTanslation(-center);

    if (radius == 0)
        radius = 1;

    if (ortho_)
        camera_.projectOrtho(-radius, +radius, -radius, +radius, 5 * radius, 400 * radius);
    else
        camera_.projectFrustum(-radius, +radius, -radius, +radius, 5 * radius, 400 * radius);

    // ����z��λ�ã���near/farƽ�������㹻�ռ�
    camera_.viewMatrix() = mat4::buildTanslation({ 0, 0, -7 * radius }) * camera_.viewMatrix();
}


void KcImPlot3d::handleMouseInput_()
{
    // �������drag�¼���ת��trackball�������������λ��
    if (ImGui::IsMouseClicked(0)) {
        auto mousePos = ImGui::GetMousePos();
        auto pivot = camera_.worldToScreen(coordSystem().center());
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
            coordSystem().zoom(factor);
        else // ��������plot
           zoom_ *= factor;
    }

    if (ImGui::IsMouseDragging(1)) {
        auto sz = ImGui::GetWindowSize();
        auto d = ImGui::GetMouseDragDelta(1);
        auto dx = d.x / sz.x;
        auto dy = -d.y / sz.y; // ��Ļ��y����������ͼ��y�����귴�򣬴˴�ȡ-d.y

        auto box = coordSystem().boundingBox();
        auto delta = box.size() * point3(dx, dy, 0);
        shift_ += delta * 0.1f;
    }
}
