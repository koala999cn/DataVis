#include "imapp/KsImApp.h"
#include "imapp/KcImNodeEditor.h"
#include "imapp/KcImActionPanel.h"
#include "imapp/KcImPropertySheet.h"
#include "imapp/KcActionNewTextData.h"
#include "imapp/KcActionNewAudioData.h"
#include "imapp/KtActionInsertNode.h"
#include "imapp/KgImWindowManager.h"
#include "imapp/KcModuleImGuiGlfw.h"
#include "imapp/KcModuleImNode.h"
#include "imapp/KcModuleImFileDialog.h"
#include "imapp/KgPipeline.h"
#include "plot/KsThemeManager.h"
#include "imgui.h"
#include "provider.h"
#include "operator.h"
#include "renderer.h"

static void initActions();
static bool showMainMenu();

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

    app.windowManager().registerWindow<KcImNodeEditor>("Node Editor");
    app.windowManager().registerWindow<KcImActionPanel>("Action Panel");
    app.windowManager().registerWindow<KcImPropertySheet>("Property Sheet");

    initActions();

    // 加载theme
    KsThemeManager::singleton().load("themes/*.json");

    app.listenPerFrame(showMainMenu);
    app.run();
    app.deinitialize();
    
    return 0;
}


void initActions()
{
    auto panel = KsImApp::singleton().windowManager().getWindow<KcImActionPanel>();
    assert(panel);

    panel->addAction("Provider", std::make_shared<KcActionNewTextData>());
    panel->addAction("Provider", std::make_shared<KcActionNewAudioData>());
    panel->addAction("Provider", std::make_shared<KtActionInsertNode<KcPvExpr>>("Math Expression"));
    panel->addAction("Provider", std::make_shared<KtActionInsertNode<KcPvFunction>>("Function"));
    panel->addAction("Provider", std::make_shared<KtActionInsertNode<KcPvStochastic>>("Stochastic"));
    panel->addAction("Provider", std::make_shared<KtActionInsertNode<KcPvOscillator>>("Oscillator"));
    panel->addAction("Provider", std::make_shared<KtActionInsertNode<KcPvAudioInput>>("Audio Input"));
    panel->addAction("Operator", std::make_shared<KtActionInsertNode<KcOpSpectrum>>("Spectrum"));
    panel->addAction("Operator", std::make_shared<KtActionInsertNode<KcOpFraming>>("Framing"));
    panel->addAction("Operator", std::make_shared<KtActionInsertNode<KcOpHist>>("Hist"));
    panel->addAction("Operator", std::make_shared<KtActionInsertNode<KcOpHistC>>("HistC"));
    panel->addAction("Operator", std::make_shared<KtActionInsertNode<KcOpSampler>>("Sampler"));
    panel->addAction("Operator", std::make_shared<KtActionInsertNode<KcOpResampler>>("Resampler"));
    panel->addAction("Operator", std::make_shared<KtActionInsertNode<KcOpFbank>>("Fbank"));
    panel->addAction("Operator", std::make_shared<KtActionInsertNode<KcOpFIR>>("FIR"));
    panel->addAction("Operator", std::make_shared<KtActionInsertNode<KcOpInterpolater>>("Interpolater"));
    panel->addAction("Operator", std::make_shared<KtActionInsertNode<KcOpWindowing>>("Windowing"));
    panel->addAction("Renderer", std::make_shared<KtActionInsertNode<KcRdPlot1d>>("Plot1d"));
    panel->addAction("Renderer", std::make_shared<KtActionInsertNode<KcRdPlot2d>>("Plot2d"));
    panel->addAction("Renderer", std::make_shared<KtActionInsertNode<KcRdPlot3d>>("Plot3d"));
    panel->addAction("Renderer", std::make_shared<KtActionInsertNode<KcRdAudioPlayer>>("Audio Player"));
}


bool showMainMenu()
{
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