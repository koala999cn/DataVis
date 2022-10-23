#include "imguix.h"
#include "KuStrUtil.h"
#include "KuDataUtil.h"
#include "KvDiscreted.h"
#include "KcSampled2d.h"


namespace kPrivate
{
    std::string seriesName_(int idx)
    {
        std::string name;
        while (true) {
            name.insert(name.begin(), 'A' + idx % 26);
            idx /= 26;

            if (idx == 0)
                break;
            --idx;
        }

        return name;
    }

    void setupTableHeader_(int dataType, int cols)
    {
        if (dataType == KuDataUtil::k_sampled_1d) {
            ImGui::TableSetupColumn("time");
            std::string str("ch-");
            for (int c = 1; c < cols; c++)
                ImGui::TableSetupColumn((str + KuStrUtil::toString(c)).c_str());
        }
        else if (dataType == KuDataUtil::k_scattered_1d) {
            for (int ch = 1; ch <= cols / 2; ch++) {
                auto chIdx = KuStrUtil::toString(ch);
                ImGui::TableSetupColumn(("X" + chIdx).c_str());
                ImGui::TableSetupColumn(("Y" + chIdx).c_str());
            }
        }
        else if (dataType == KuDataUtil::k_scattered_2d) {
            for (int ch = 1; ch <= cols / 3; ch++) {
                auto chIdx = KuStrUtil::toString(ch);
                ImGui::TableSetupColumn(("X" + chIdx).c_str());
                ImGui::TableSetupColumn(("Y" + chIdx).c_str());
                ImGui::TableSetupColumn(("Z" + chIdx).c_str());
            }
        }
        else if (dataType == KuDataUtil::k_series) {
            for (int c = 0; c < cols; c++)
                ImGui::TableSetupColumn(seriesName_(c).c_str());
        }
        else {
            for (int c = 1; c <= cols; c++)
                ImGui::TableSetupColumn(KuStrUtil::toString(c).c_str());
        }
    }

}


namespace ImGuiX
{
	void AddLineDashed(const ImVec2& a, const ImVec2& b, ImU32 col, float thickness, unsigned int num_segments, unsigned int on_segments, unsigned int off_segments)
	{
        if ((col >> 24) == 0)
            return;

        auto dl = ImGui::GetWindowDrawList();
        int on = 0, off = 0;
        ImVec2 dir = (b - a) / num_segments;
        for (int i = 0; i <= num_segments; i++)
        {
            ImVec2 point(a + dir * i);
            if (on < on_segments) {
                dl->_Path.push_back(point);
                on++;
            }
            else if (on == on_segments && off == 0) {
                dl->_Path.push_back(point);
                dl->PathStroke(col, false, thickness);
                off++;
            }
            else if (on == on_segments && off < off_segments) {
                off++;
            }
            else {
                dl->_Path.resize(0);
                dl->_Path.push_back(point);
                on = 1;
                off = 0;
            }
        }
        dl->PathStroke(col, false, thickness);
	}


    void drawColorBar(std::vector<color4f>& colors, const ImVec2& startPos, const ImVec2& blockSize, float blockSpace)
    {
        float step = blockSize.x + blockSpace;
        ImGui::Dummy(ImVec2(colors.size() * step - blockSpace, blockSize.y));
        auto blockMin = startPos;
        for (unsigned i = 0; i < colors.size(); i++) {
            auto blockMax = blockMin + blockSize;
            ImGui::GetWindowDrawList()->AddRectFilled(
                blockMin, blockMax, ImColor((ImVec4&)colors[i]));
            blockMin.x += step;
        }
    }


    void showDataTable(const KvData& data)
    {
        if (data.isDiscreted()) {
            auto& disc = (const KvDiscreted&)data;
            if (disc.isScattered()) {
                showDataTable(disc.dim() == 1 ? 
                    KuDataUtil::k_scattered_1d : KuDataUtil::k_scattered_2d, 
                    disc.size(),
                    disc.channels() * (disc.dim() + 1),
                    [&disc](unsigned r, unsigned c) {
                        auto ch = c / (disc.dim() + 1);
                        auto idx = c % (disc.dim() + 1);
                        return disc.pointAt(r, ch).at(idx);
                    });
            }
            else if (disc.isSampled()) {
                if (disc.dim() == 1) {
                    if (disc.step(0) == 1) { // series 
                        showDataTable(KuDataUtil::k_series, disc.size(), disc.channels(),
                            [&disc](unsigned r, unsigned c) {
                                return disc.pointAt(r, c).at(1);
                            });
                    }
                    else { // sampled1d
                        showDataTable(KuDataUtil::k_sampled_1d, disc.size(), disc.channels() + 1,
                            [&disc](unsigned r, unsigned c) {
                                if (c == 0)
                                    return disc.pointAt(r, 0).at(0);
                                else
                                    return disc.valueAt(r, c - 1);
                            });
                    }
                }
                else if (disc.dim() == 2) {
                    auto samp2d = (const KcSampled2d&)disc;
                    if (disc.step(0) == 1 && disc.step(1) == 1) { // matrix
                        showDataTable(KuDataUtil::k_matrix, disc.size(0), disc.size(1),
                            [&samp2d](unsigned r, unsigned c) {
                                return samp2d.value(r, c, 0);
                            });
                    }
                    else { // sampled2d
                        showDataTable(KuDataUtil::k_sampled_2d, disc.size(0) + 1, disc.size(1) + 1,
                            [&samp2d](unsigned r, unsigned c) -> double {
                                if (r == 0 && c == 0)
                                    return std::numeric_limits<double>::quiet_NaN();
                                else if (r == 0)
                                    return samp2d.sampling(1).indexToX(c - 1);
                                else if (c == 0)
                                    return samp2d.sampling(0).indexToX(r - 1);
                                else 
                                    return samp2d.value(r - 1, c - 1, 0);
                            });
                    }
                }
                
            }
        }
    }


    void showDataTable(int type, const matrixd& data, bool rowMajor)
    {
        if (data.empty())
            return;

        unsigned rows = data.size();
        unsigned cols = data[0].size();
        if (rowMajor)
            std::swap(rows, cols);

        showDataTable(type, rows, cols, [&data, rowMajor](unsigned r, unsigned c) {
            return rowMajor ? data[c][r] : data[r][c];
            });
    }


    void showDataTable(int type, unsigned rows, unsigned cols, std::function<double(unsigned, unsigned)> fn)
    {
        const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
        const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
        static ImGuiTableFlags flags =
            ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY |
            ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter |
            ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable;
        static int freeze_cols = 1;
        static int freeze_rows = 1;

        if (cols >= 64)
            cols = 63; // TODO: ImGui要求总列数不超过64，此处首列留给行序号

        if (ImGui::BeginTable("RawData", cols + 1, flags)) {

            ImGui::TableSetupScrollFreeze(freeze_cols, freeze_rows);
            ImGui::TableSetupColumn("No.", ImGuiTableColumnFlags_NoHide); // Make the first column not hideable to match our use of TableSetupScrollFreeze()
            kPrivate::setupTableHeader_(type, cols);
            ImGui::TableHeadersRow();

            for (int r = 0; r < rows; r++) {

                ImGui::TableNextRow();
                for (int c = 0; c <= cols; c++) {

                    if (!ImGui::TableSetColumnIndex(c) && c > 0)
                        continue;

                    if (c == 0)
                        ImGui::Text(KuStrUtil::toString(r + 1).c_str()); // 打印行号
                    else {
                        auto val = fn(r, c - 1);
                        ImGui::Text(KuStrUtil::toString(val).c_str());
                    }
                }
            }
            ImGui::EndTable();
        }
    }
}