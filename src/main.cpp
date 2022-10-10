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

    panel->addAction("Provider", std::make_shared<KcActionNewTextData>());
    panel->addAction("Renderer", std::make_shared<KtActionInsertNode<KcRdPlot1d>>("Plot1d"));

    app.listenPerFrame([&editor]() -> bool {

        if (ImGui::BeginMainMenuBar()) {

            KsImApp::singleton().windowManager().showMenu("View");

            if (ImGui::BeginMenu("Pileline")) {

                if (ImGui::MenuItem("Start")) 
                    editor->start();

                if (ImGui::MenuItem("Stop"))
                    editor->stop();

                ImGui::EndMenu();
            }
            
            ImGui::EndMainMenuBar();
        }

        KsImApp::singleton().windowManager().update();

        if (editor->status() == KcImNodeEditor::k_busy)
            editor->stepFrame();

        return true;
        });

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