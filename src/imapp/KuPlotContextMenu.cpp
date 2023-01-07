#include "KuPlotContextMenu.h"
#include "imgui.h"
#include "KuStrUtil.h"
#include "KcActionShowFileDialog.h"
#include "plot/KvPaint.h"
#include "plot/KvPlot.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"


void KuPlotContextMenu::open()
{
	ImGui::OpenPopup("##CONTEXT");
}


void KuPlotContextMenu::update(KvPlot* plot)
{
    static int lag(0);
    static KcActionShowFileDialog fd(KcActionShowFileDialog::KeType::k_save,
        "Export As", "Image file (*.png;*.jpg;*.bmp){.png,.jpg,.bmp}");

    if (ImGui::BeginPopup("##CONTEXT")) {
        if (ImGui::MenuItem("Export to Image...")) {
            ImGui::CloseCurrentPopup();
            fd.trigger();
        }
        ImGui::EndPopup();
    }

    if (lag > 2) { // NB: ���Ӻ�2֡ץȡͼƬ�������ץ���ļ��Ի���
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
    }
    else if (lag > 0)
        lag++;

    if (fd.triggered()) {
        fd.update();
        if (fd.done())
            lag = 1;
    }
}
