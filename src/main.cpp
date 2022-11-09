﻿#include "imapp/KsImApp.h"
#include "imgui.h"
#include "imapp/KcImNodeEditor.h"
#include "imapp/KcImActionPanel.h"
#include "imapp/KcImPropertySheet.h"
#include "imapp/KcActionNewTextData.h"
#include "imapp/KtActionInsertNode.h"
#include "imapp/KgImWindowManager.h"
#include "imapp/KcModuleImGuiGlfw.h"
#include "imapp/KcModuleImNode.h"
#include "imapp/KcModuleImFileDialog.h"
#include "imapp/KgPipeline.h"
#include "prov/KcPvAudioInput.h"
#include "prov/KcPvExpr.h"
#include "render/KcRdPlot1d.h"
#include "render/KcRdPlot2d.h"
#include "render/KcRdPlot3d.h"
#include "render/KcRdAudioPlayer.h"
#include "op/KcOpSpectrum.h"
#include "op/KcOpFraming.h"
#include "op/KcOpHist.h"
#include "op/KcOpSampler.h"
#include "plot/KsThemeManager.h"


int main_(int, char**)
{
    auto& app = KsImApp::singleton();
    
    auto imgui = app.registerModule<KcModuleImGuiGlfw>(1024, 768, "DataVis");
    auto imnode = app.registerModule<KcModuleImNode>();
    auto imfiledialog = app.registerModule<KcModuleImFileDialog>();
    app.setDependent(imnode, imgui);
    app.setDependent(imfiledialog, imgui);

    if (!app.initialize())
        return 1;

    auto editor = app.windowManager().registerWindow<KcImNodeEditor>("Node Editor");
    auto panel = app.windowManager().registerWindow<KcImActionPanel>("Action Panel");
    auto sheet = app.windowManager().registerWindow<KcImPropertySheet>("Property Sheet");

    panel->addAction("Provider", std::make_shared<KcActionNewTextData>());
    panel->addAction("Provider", std::make_shared<KtActionInsertNode<KcPvExpr>>("Math Expression"));
    panel->addAction("Provider", std::make_shared<KtActionInsertNode<KcPvAudioInput>>("Audio Input"));
    panel->addAction("Operator", std::make_shared<KtActionInsertNode<KcOpSpectrum>>("Spectrum"));
    panel->addAction("Operator", std::make_shared<KtActionInsertNode<KcOpFraming>>("Framing"));
    panel->addAction("Operator", std::make_shared<KtActionInsertNode<KcOpHist>>("Hist"));
    panel->addAction("Operator", std::make_shared<KtActionInsertNode<KcOpSampler>>("Sampler"));
    panel->addAction("Renderer", std::make_shared<KtActionInsertNode<KcRdPlot1d>>("Plot1d"));
    panel->addAction("Renderer", std::make_shared<KtActionInsertNode<KcRdPlot2d>>("Plot2d"));
    panel->addAction("Renderer", std::make_shared<KtActionInsertNode<KcRdPlot3d>>("Plot3d"));
    panel->addAction("Renderer", std::make_shared<KtActionInsertNode<KcRdAudioPlayer>>("Audio Player"));

    // 加载theme
    KsThemeManager::singleton().load("themes/*.json");

    app.listenPerFrame([&editor]() -> bool {

        if (ImGui::BeginMainMenuBar()) {

            KsImApp::singleton().windowManager().showMenu("View");
            auto& pipe = KsImApp::singleton().pipeline();

            if (ImGui::BeginMenu("Pileline")) {

                if (ImGui::MenuItem("Start", nullptr, nullptr, !pipe.running()))
                    pipe.start();

                if (ImGui::MenuItem("Stop", nullptr, nullptr, pipe.running()))
                    pipe.stop();

                ImGui::EndMenu();
            }
            
            ImGui::EndMainMenuBar();
        }

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