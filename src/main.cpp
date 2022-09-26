#include "imapp/KsImApp.h"
#include "imgui.h"
#include "imapp/KgImMenu.h"
#include "imapp/KgImWindowManager.h"
#include "imapp/KcImNodeEditor.h"
#include "imapp/KcImActionPanel.h"
#include "imapp/KcActionLoadText.h"


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

    return true;
}


int main_(int, char**)
{
    auto& app = KsImApp::singleton();

    if (!app.init(1024, 768, "DataVis"))
        return 1;

    auto editor = std::make_shared<KcImNodeEditor>("Node Editor");
    auto panel = std::make_shared<KcImActionPanel>("Action Panel");
    panel->addAction("Provider", std::make_shared<KcActionLoadText>());

    kPrivate::wm.registerInstance(editor);
    kPrivate::wm.registerInstance(panel);

    app.listenPerFrame(update);
    app.run();

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