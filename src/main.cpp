#include "imapp/KsImApp.h"
#include "imgui.h"
#include "imapp/KcImNodeEditor.h"
#include "imapp/KcImActionPanel.h"
#include "imapp/KcActionNewTextData.h"
#include "imapp/KtActionInsertNode.h"
#include "imapp/KgImWindowManager.h"
#include "imapp/KcModuleImGuiGlfw.h"
#include "imapp/KcModuleImNode.h"
#include "imapp/KcModuleImFileDialog.h"
#include "imapp/KcModuleImPlot.h"
#include "render/KcRdPlot1d.h"

#include "imapp/KcImPlot3d.h"

bool update()
{
    // show menu bar of main window
    if (ImGui::BeginMainMenuBar()) {
        //if (ImGui::BeginMenu("View")) {
        //    ImGui::ShowStyleSelector("Style");
        //    ImGui::EndMenu();
        //}

        KsImApp::singleton().windowManager().showMenu("View");
        ImGui::EndMainMenuBar();
    }

    KsImApp::singleton().windowManager().update();

    return true;
}


int main_(int, char**)
{
    auto& app = KsImApp::singleton();
    
    auto imgui = app.registerModule<KcModuleImGuiGlfw>(1024, 768, "DataVis");
    auto imnode = app.registerModule<KcModuleImNode>();
    auto imfiledialog = app.registerModule<KcModuleImFileDialog>();
    auto implot = app.registerModule<KcModuleImPlot>();
    app.setDependent(imnode, imgui);
    app.setDependent(imfiledialog, imgui);
    app.setDependent(implot, imgui);

    if (!app.initialize())
        return 1;

    auto editor = app.windowManager().registerStatic<KcImNodeEditor>("Node Editor");
    auto panel = app.windowManager().registerStatic<KcImActionPanel>("Action Panel");
    app.windowManager().registerStatic<KcImPlot3d>("Plot3d");
    panel->addAction("Provider", std::make_shared<KcActionNewTextData>());
    panel->addAction("Renderer", std::make_shared<KtActionInsertNode<KcRdPlot1d>>("Plot1d"));

    app.listenPerFrame(update);
    app.run();

    app.deinitialize();
    
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