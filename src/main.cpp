﻿#include "imapp/KsImApp.h"
#include "imgui.h"
#include "imapp/KgImMenu.h"
#include "imnodes/imnodes.h"
#include "imapp/KgImWindowManager.h"


namespace kPrivate
{
    enum {
        k_style_classic, k_style_dark, k_style_light
    };

    static int style_colors = k_style_dark;
    static KgImWindowManager wm;
}


static void showMainMenuBar()
{
    using namespace kPrivate;

    
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("View")) {
            ImGui::ShowStyleSelector("Style");
            ImGui::EndMenu();
        }

        wm.showMenu("Window");
        ImGui::EndMainMenuBar();
    }
}


bool update()
{
    showMainMenuBar();

    kPrivate::wm.draw();

    ImGui::BeginChild("simple node editor");

    ImNodes::BeginNodeEditor();
    ImNodes::BeginNode(1);

    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("simple node :)");
    ImNodes::EndNodeTitleBar();

    ImNodes::BeginInputAttribute(2);
    ImGui::Text("input");
    ImNodes::EndInputAttribute();

    ImNodes::BeginOutputAttribute(3);
    ImGui::Indent(40);
    ImGui::Text("output");
    ImNodes::EndOutputAttribute();

    ImNodes::EndNode();
    ImNodes::EndNodeEditor();

    ImGui::EndChild();

    return true;
}


int main_(int, char**)
{
    auto& app = KsImApp::singleton();

    if (!app.init(1024, 768, "DataVis"))
        return 1;

    ImNodes::CreateContext();
    ImNodes::SetNodeGridSpacePos(1, ImVec2(200.0f, 200.0f));

    app.listenPerFrame(update);
    app.run();

    ImNodes::DestroyContext();
    app.shutdown();
    
    return 0;
}

#ifdef _WIN32

#include <Windows.h>

int APIENTRY WinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE hInstPrev, _In_ PSTR cmdline, _In_ int cmdshow)
{
    return main_(__argc, __argv);
}

#else

int main(int argc, char** argv)
{
    return main_(argc, argv);
}

#endif // WIN32_