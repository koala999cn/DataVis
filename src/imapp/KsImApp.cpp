#include "KsImApp.h"
#include <stdio.h>
#include <glfw3.h> // Will drag system OpenGL headers
#include <assert.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "KcModuleImGuiGlfw.h"
#include "KgPipeline.h"
#include "KgImWindowManager.h"


KsImApp::KsImApp()
{
    winMgr_ = std::make_unique<KgImWindowManager>();
    pipeline_ = std::make_unique<KgPipeline>();
}


KsImApp::~KsImApp()
{

}


void KsImApp::run()
{
    for (auto ls : lsStart_)
        if (!ls()) return;

    auto imgui = getModule<KcModuleImGuiGlfw>();
    auto window = imgui->glfwWindow();
    while (!glfwWindowShouldClose(window)) {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        if (pipeline_->running())
            pipeline_->stepFrame();

        drawFrame_();

        // 无焦点时，放慢更新节奏
        //if (!glfwGetWindowAttrib(window, GLFW_FOCUSED)) 
        //    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    for (auto ls : lsFinish_)
        ls();
}


void KsImApp::quit()
{
    auto imgui = getModule<KcModuleImGuiGlfw>();
    glfwSetWindowShouldClose(imgui->glfwWindow(), 1);
}


void KsImApp::drawFrame_()
{
    ImVec4 bkclr = ImVec4(0.45f, 0.55f, 0.60f, 1.00f); // 主窗口的背景色

    auto imgui = getModule<KcModuleImGuiGlfw>();
    auto window = imgui->glfwWindow();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    for (auto ls : lsUpdate_)
        if (!ls()) {
            quit();
            break;
        }

    // 最小化时，不进行实际渲染
    if (glfwGetWindowAttrib(window, GLFW_ICONIFIED)) {
        ImGui::EndFrame();
    }
    else { // Rendering 

        winMgr_->update();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(bkclr.x, bkclr.y, bkclr.z, bkclr.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
    //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        if(ImGui::GetDrawData())
            ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }

    glfwSwapBuffers(window);
}