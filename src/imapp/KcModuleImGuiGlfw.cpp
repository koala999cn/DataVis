#include "KcModuleImGuiGlfw.h"
#include <glad.h>
#include <glfw3.h> // Will drag system OpenGL headers
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"


KcModuleImGuiGlfw::KcModuleImGuiGlfw(int w, int h, const char* title)
    : KvModule("ImGui_glfw3")
    , width_(w)
    , height_(h)
    , title_(title)
{

}


bool KcModuleImGuiGlfw::initialize()
{
    if (!initGl_())
        return false;

    if (!initImGui_()) {
        deinitGl_();
        return false;
    }

    return true;
}


void KcModuleImGuiGlfw::deinitialize()
{
    deinitImGui_();
    deinitGl_();
}


bool KcModuleImGuiGlfw::postInitialize()
{
    glfwShowWindow(glfwWin_);
    return true;
}


bool KcModuleImGuiGlfw::initGl_()
{
    // glfwSetErrorCallback(kPrivate::glfw_error_callback);
    
    // Setup window
    if (!glfwInit())
        return false;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    glslVer_ = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    glslVer_ = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.3 + GLSL 130
    // NB: 初始化opengl 3.3，以便使用多实例渲染功能
    glslVer_ = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // NB: 为了确保3.3版本下固定管线可用
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE); // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    glfwWindowHint(GLFW_SAMPLES, 4); // 启用多重采样

    // Create window with graphics context
    auto window = glfwCreateWindow(width_, height_, title_.c_str(), nullptr, nullptr);
    if (window == nullptr) {
        glfwTerminate();
        return false;
    }

    { // TODO:
        GLFWimage icon;
        //icon.pixels = stbi_load_from_memory((const stbi_uc*)Icon_data, Icon_size, &icon.width, &icon.height, nullptr, 4);
        //glfwSetWindowIcon(window, 1, &icon);
        //free(icon.pixels);
    }

    // TODO: if (maximize) 
    glfwMaximizeWindow(window);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return false;
    }

    auto vendor = glGetString(GL_VENDOR);
    auto renderer = glGetString(GL_RENDERER);
    auto ver = glGetString(GL_VERSION);
    auto ext = glGetString(GL_EXTENSIONS);

    //if (!GLAD_GL_VERSION_3_3)
     //   return false;

    glfwWin_ = window;

    return true;
}


bool KcModuleImGuiGlfw::initImGui_()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    if (ImGui::CreateContext() == nullptr)
        return false;

    // io设置须放在其他初始化之前，否则ImGui初始化不完全，后面程序运行会crack
    ImGuiIO& io = ImGui::GetIO();
    // TODO: std::string iniFileName = tracy::GetSavePath("imgui.ini");
    //io.IniFilename = iniFileName.c_str();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // 先设置该标记，以便初始化Viewports支持


    // Setup Platform/Renderer backends
    if (!ImGui_ImplGlfw_InitForOpenGL(glfwWin_, true)) {
        ImGui::DestroyContext();
        return false;
    }

    if (!ImGui_ImplOpenGL3_Init(glslVer_.c_str())) {
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        return false; // TODO: shutdown
    }

    io.ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable; // 默认去除Viewports

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    ImFont* font = io.Fonts->AddFontFromFileTTF("font/simsun.ttc", 13.0f, NULL, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
    IM_ASSERT(font != NULL);

    return true;
}


void KcModuleImGuiGlfw::deinitImGui_()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}


void KcModuleImGuiGlfw::deinitGl_()
{
    glfwDestroyWindow(glfwWin_);
    glfwTerminate();
    glfwWin_ = nullptr;
}

