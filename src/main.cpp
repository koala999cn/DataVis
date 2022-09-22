﻿#include "imapp/KsImApp.h"
#include "imgui.h"
#include "imapp/KgImMenu.h"


static void showMainMenuBar()
{
    KgImMenu view("View");
    auto& item = view.addItem("Style", nullptr);
    KgImMenu::KpItem subItem;
    subItem.label = "Classic";
    subItem.handler = []() {
        ImGui::StyleColorsClassic();
    };
    subItem.selected = false;
    item.subItems.push_back(subItem);
    subItem.label = "Dark";
    subItem.handler = []() {
        ImGui::StyleColorsDark();
    };
    item.subItems.push_back(subItem);
    subItem.label = "Light";
    subItem.handler = []() {
        ImGui::StyleColorsLight();
    };
    item.subItems.push_back(subItem);


    if (ImGui::BeginMainMenuBar()) {
        view.apply();
        ImGui::EndMainMenuBar();
    }
}


bool update()
{
    static bool show_demo_window, show_another_window;
    
    auto& style = ImGui::GetStyle();

    showMainMenuBar();
    
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::BeginChild("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("background color", (float*)&style.Colors[ImGuiCol_WindowBg]); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::EndChild();
    }

    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            KsImApp::singleton().quit();
        ImGui::End();
    }

    return true;
}


int main_(int, char**)
{
    auto& app = KsImApp::singleton();

    if (!app.init(1024, 768, "DataVis"))
        return 1;

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