#include "KcImPlot3d.h"
#include "KcImPaint.h"
#include "imgui.h"


KcImPlot3d::KcImPlot3d(const std::string_view& name)
    : KvPlot3d(std::make_shared<KcImPaint>(camera_))
    , KvImWindow(name)
    , trackball_(orient_)
{

}


void KcImPlot3d::update() 
{
    // 设置窗口背景为plot的背景色
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(bkclr_.r(), bkclr_.g(), bkclr_.b(), bkclr_.a()));

    KvImWindow::update();

    ImGui::PopStyleColor();
}


void KcImPlot3d::updateImpl_()
{
    // 更新摄像机的视图
    auto pos = ImGui::GetWindowPos();
    auto sz = ImGui::GetWindowSize();
    camera_.setViewport(pos.x, pos.y, sz.x, sz.y);

    // 处理鼠标drag事件，转动trackball，更新摄像机方位角
    if (ImGui::IsMouseClicked(0)) {
        auto mousePos = ImGui::GetMousePos();      
        trackball_.reset({ (pos.x + sz.x) / 2, (pos.y + sz.y) / 2 }, { sz.x / 2, sz.y / 2 });
        trackball_.start(mousePos.x, mousePos.y);
    }
    if (ImGui::IsMouseDragging(0)) {
        auto d = ImGui::GetMouseDragDelta();
        trackball_.delta(d.x, d.y);
    }

    KvPlot3d::update();

    // 设置只有拖动标题栏才能移动窗口
    auto& style = ImGui::GetStyle();
    auto titlebarHeight = ImGui::GetFontSize() + style.FramePadding.y * 2;
    auto innerMin = ImGui::GetWindowPos();
    auto innerMax = ImGui::GetWindowSize();
    innerMax.x += innerMin.x, innerMax.y += innerMin.y;
    innerMin.y += titlebarHeight;
    noMove_ = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(innerMin, innerMax);
}


int KcImPlot3d::flags() const
{
    return noMove_ ? KvImWindow::flags() | ImGuiWindowFlags_NoMove : KvImWindow::flags();
}


void KcImPlot3d::autoProject_()
{
    auto lower = coordSystem().lower();
    auto upper = coordSystem().upper();
    auto center = lower + (upper - lower) / 2;
    double radius = coordSystem().diag() / 2;

    auto zoom = getZoom();
    auto scale = getScale();
    auto rot = getRotate();
    auto shift = getShift();
    if (!isIsometric()) {
        zoom *= 2 * radius / sqrt(3.);
        scale /= (upper - lower);
    }
    scale *= zoom;

    //camera_.viewMatrix() = camera_.viewMatrix().buildTanslation(-center);
 /*   vl::Transform tr;
    //tr.rotate(-90, 1, 0, 0); // 旋转+z轴由向外为向上, +y轴由向上为向内
    tr.translate(vl::vec3(-center)); // 把物理坐标AABB的中心点调整为摄像机坐标的原点
    tr.scale(scale.x(), scale.y(), scale.z());
    tr.rotate(rot.x(), 1, 0, 0);
    tr.rotate(rot.y(), 0, 1, 0);
    tr.rotate(rot.z(), 0, 0, 1);
    tr.translate(vl::vec3(shift));
    tr.translate(0, 0, -7 * radius); // 调整z轴位置，给near/far平面留出足够空间
    camera->setViewMatrix(tr.localMatrix());*/

    camera_.lookAt({ 7 * radius, 7 * radius, 7 * radius }, center, { 0, 1, 0 });

    camera_.viewMatrix() = camera_.viewMatrix() * mat4::buildTransform({ 0, 0, 0 }, { 1, 1, 1 }, orient_);

    if (radius == 0)
        radius = 1;

    if (ortho_)
        camera_.projectOrtho(-radius, +radius, -radius, +radius, 5 * radius, 400 * radius);
    else
        camera_.projectFrustum(-radius, +radius, -radius, +radius, 5 * radius, 400 * radius);
}