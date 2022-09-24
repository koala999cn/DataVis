#include "imapp/KsImApp.h"
#include "imgui.h"
#include "imapp/KgImMenu.h"
#include "imnodes/imnodes.h"
#include "imapp/KgImWindowManager.h"
#include "imapp/KcImNodeEditor.h"
#include "imapp/KcImActionPanel.h"
#include "ImFileDialog/ImFileDialog.h"
#include <glad.h>
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

    ImNodes::CreateContext();
    ImNodes::SetNodeGridSpacePos(1, ImVec2(200.0f, 200.0f));

    ifd::FileDialog::Instance().CreateTexture = [](uint8_t* data, int w, int h, char fmt) -> void* {
        GLuint tex;

        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, (fmt == 0) ? GL_BGRA : GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        return (void*)tex;
    };
    ifd::FileDialog::Instance().DeleteTexture = [](void* tex) {
        GLuint texID = (GLuint)tex;
        glDeleteTextures(1, &texID);
    };

    auto editor = std::make_shared<KcImNodeEditor>("Node Editor");
    auto panel = std::make_shared<KcImActionPanel>("Action Panel");
    panel->addAction("Provider", std::make_shared<KcActionLoadText>("Text Data"));

    kPrivate::wm.registerInstance(editor);
    kPrivate::wm.registerInstance(panel);

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