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
    // ���ô��ڱ���Ϊplot�ı���ɫ
    ImGui::PushStyleColor(ImGuiCol_ChildBg, (const ImVec4&)background().color); // TODO: check the style

    if (ImGui::BeginChild("##", ImVec2(0, 0), false, ImGuiWindowFlags_NoMove)) {

        // �������������ͼ
        auto pos = ImGui::GetWindowPos();
        auto sz = ImGui::GetWindowSize();
        paint().setViewport({ { pos.x, pos.y }, { pos.x + sz.x, pos.y + sz.y } });

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
        KtVector4<float_t> pivot(coord().center(), 1);
        if (coord().axisInversed()) // NB: ���ڴ˴�paint�ı任�����ջ��δ������updateû�е��ã��������ֶ����������ύ������ͬ
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