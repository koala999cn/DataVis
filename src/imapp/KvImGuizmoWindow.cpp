#include "KvImGuizmoWindow.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "KtCamera.h" 


KvImGuizmoWindow::KvImGuizmoWindow(const std::string_view& name)
    : KvImWindow(name)
{
    KtCamera<float> cam;

    vec3f eye{ 5, 5, 5 };
    vec3f at{};
    vec3f up{ 0, 1, 0 };
    cam.lookAt(eye, at, up);
    viewMat_ = cam.viewMatrix().getTranspose(); // ImGuizmo使用的矩阵与KtCamera使用的为转置关系

    ImGuiIO& io = ImGui::GetIO();
    cam.projectPerspective(45, io.DisplaySize.x / io.DisplaySize.y, 5, 100.f);
    projMat_ = cam.projMatrix().getTranspose();
}


void KvImGuizmoWindow::update()
{
    ImGui::SetNextWindowSizeConstraints(
        ImVec2{ minSize_[0], minSize_[1] },
        ImVec2{ maxSize_[0], maxSize_[1] }
    );

    auto f = noMove_ ? flags() | ImGuiWindowFlags_NoMove : flags();
    if (ImGui::Begin(name().c_str(), &visible_, f) && visible()) {

        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, 
            ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

        updateImpl_();

        noMove_ = ImGui::IsWindowHovered(); 
    }

    ImGui::End();
}


void KvImGuizmoWindow::updateImpl_()
{
    int gizmoCount = 3;
    float camDistance = 8.f;
    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);

    float objectMatrix[16] = {
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 0.f, 1.f 
    };

    static const float identityMatrix[16] =
    { 1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 0.f, 1.f };

    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
    static bool useSnap = false;
    static float snap[3] = { 1.f, 1.f, 1.f };
    static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
    static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
    static bool boundSizing = false;
    static bool boundSizingSnap = false;

    ImGuiIO& io = ImGui::GetIO();
    float viewManipulateRight = io.DisplaySize.x;
    float viewManipulateTop = 0;

    viewManipulateRight = ImGui::GetWindowPos().x + ImGui::GetWindowWidth();
    viewManipulateTop = ImGui::GetWindowPos().y;

    ImGuizmo::DrawGrid(viewMat_.data(), projMat_.data(), identityMatrix, 100.f);
    ImGuizmo::DrawCubes(viewMat_.data(), projMat_.data(), objectMatrix, 1);
    //ImGuizmo::Manipulate(viewMat_.data(), projMat_.data(),
    //   mCurrentGizmoOperation, mCurrentGizmoMode, objectMatrix, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);

    ImGuizmo::ViewManipulate(viewMat_.data(), camDistance, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);

    ImGui::DragFloat4("row0", &viewMat_[0]);
    ImGui::DragFloat4("row1", &viewMat_[4]);
    ImGui::DragFloat4("row2", &viewMat_[8]);
    ImGui::DragFloat4("row2", &viewMat_[12]);
}
