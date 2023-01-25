#include "KuPlotContextMenu.h"
#include "imgui.h"
#include "KuStrUtil.h"
#include "KcActionShowFileDialog.h"
#include "plot/KvPaint.h"
#include "plot/KvPlot.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"


namespace kPrivate
{
    std::string popupId(KvPlot* plot) {
        return "##" + KuStrUtil::toString(plot);
    }
}


void KuPlotContextMenu::update(KvPlot* plot)
{
    static int lag(0);
    static KcActionShowFileDialog fd(KcActionShowFileDialog::KeType::k_save,
        "Export As", "Image file (*.png;*.jpg;*.bmp){.png,.jpg,.bmp}");
    static KvPlot* activePlot = nullptr;

    // 当有其他窗口激活时，跳过后面的处理
    if (activePlot && activePlot != plot)
        return;

    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) 
        && ImGui::IsMouseReleased(ImGuiMouseButton_Right)
        && activePlot == nullptr)
        ImGui::OpenPopup("##CONTEXT");

    if (ImGui::BeginPopup("##CONTEXT")) {
        if (ImGui::MenuItem("Export to Image...")) {
            ImGui::CloseCurrentPopup();
            activePlot = plot;
            fd.trigger();
        }
        ImGui::EndPopup();
    }

    if (lag > 2) { // NB: 须延后2帧抓取图片，否则会抓到文件对话框
        auto pos = ImGui::GetWindowPos();
        auto sz = ImGui::GetWindowSize();
        std::vector<std::int32_t> pixels(sz.x * sz.y);
        plot->paint().grab(pos.x, pos.y, sz.x, sz.y, pixels.data());
        stbi_flip_vertically_on_write(1);

        auto& filepath = fd.result();
        if (KuStrUtil::endWith(filepath, ".bmp", true)) {
            stbi_write_bmp(filepath.c_str(), sz.x, sz.y, 4, pixels.data());
        }
        else if (KuStrUtil::endWith(filepath, ".jpg", true)) {
            stbi_write_jpg(filepath.c_str(), sz.x, sz.y, 4, pixels.data(), 0.75);
        }
        else {
            stbi_write_png(filepath.c_str(), sz.x, sz.y, 4, pixels.data(), sz.x * sizeof(std::int32_t));
        }

        lag = 0;
        activePlot = nullptr;
    }
    else if (lag > 0)
        lag++;

    if (fd.triggered()) {
        fd.update();
        if (fd.done())
            lag = 1;
        else if (fd.cancelled())
            activePlot = nullptr;
    }
}
