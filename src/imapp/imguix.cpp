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

    std::vector<std::string> makeTableHeaders_(int dataType, int cols)
    {
        std::vector<std::string> headers;
        headers.reserve(cols);

        if (dataType == KuDataUtil::k_sampled_1d) {
            headers.push_back("time");
            std::string str("ch-");
            for (int c = 1; c < cols; c++)
                headers.push_back((str + KuStrUtil::toString(c)).c_str());
        }
        else if (dataType == KuDataUtil::k_scattered_1d) {
            for (int ch = 1; ch <= cols / 2; ch++) {
                auto chIdx = KuStrUtil::toString(ch);
                headers.push_back(("X" + chIdx).c_str());
                headers.push_back(("Y" + chIdx).c_str());
            }
        }
        else if (dataType == KuDataUtil::k_scattered_2d) {
            for (int ch = 1; ch <= cols / 3; ch++) {
                auto chIdx = KuStrUtil::toString(ch);
                headers.push_back(("X" + chIdx).c_str());
                headers.push_back(("Y" + chIdx).c_str());
                headers.push_back(("Z" + chIdx).c_str());
            }
        }
        else if (dataType == KuDataUtil::k_series) {
            for (int c = 0; c < cols; c++)
                headers.push_back(seriesName_(c).c_str());
        }
        else if (dataType == KuDataUtil::k_sampled_2d) {
            headers.push_back("dx");
            for (int c = 1; c <= cols; c++)
                headers.push_back(KuStrUtil::toString(c).c_str());
        }
        else {
            for (int c = 1; c <= cols; c++)
                headers.push_back(KuStrUtil::toString(c).c_str());
        }

        return headers;
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
        auto headers = kPrivate::makeTableHeaders_(type, std::min(cols, 66u)); // 66为2与3的公倍数
        headers.insert(headers.begin(), "NO.");
        auto fnShow = [&headers, &fn, cols, type](unsigned r, unsigned c) {
            if (c == 0) { // show the row-index
                if (type == KuDataUtil::k_sampled_2d) {
                    if (r == 0)
                        ImGui::Text("dy");
                    else 
                        ImGui::Text("%d", r);
                }
                else {
                    ImGui::Text("%d", r + 1);
                }
            }
            else {
                ImGui::Text("%g", fn(r, c - 1));
            }
        };

        showLargeTable(rows, cols + 1, fnShow, 1, 1, headers);
    }


    void showLargeTable(unsigned rows, unsigned cols, 
        std::function<void(unsigned, unsigned)> fnShow, 
        unsigned freezeCols, unsigned freeszRows,
        const std::vector<std::string>& headers)
    {
        const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("ABC").x;
        const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
        const static ImGuiTableFlags flags =
            ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY |
            ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter |
            ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable;

        if (cols > 64)
            cols = 64; // TODO: ImGui要求总列数不超过64

        if (ImGui::BeginTable("LargeTable", cols, flags)) {

            ImGui::TableSetupScrollFreeze(freezeCols, freeszRows);

            if (!headers.empty()) {
                for (unsigned c = 0; c < cols; c++)
                    ImGui::TableSetupColumn(headers[c].c_str());
                ImGui::TableHeadersRow();
            }

            ImGuiListClipper clipper;
            clipper.Begin(rows);
            while (clipper.Step()) {
                for (int r = clipper.DisplayStart; r < clipper.DisplayEnd; r++) {
                    ImGui::TableNextRow();
                    for (int c = 0; c < cols; c++) 
                        if (ImGui::TableSetColumnIndex(c))
                            fnShow(r, c);
                }
            }

            ImGui::EndTable();
        }
    }


    bool prefixCheckbox(const char* label, bool* v)
    {
        auto w = ImGui::CalcItemWidth();

        bool res = ImGui::Checkbox(label, v);

        ImGui::PushItemWidth(w - ImGui::GetItemRectSize().x - ImGui::GetStyle().ItemSpacing.x);
        ImGui::SameLine();

        return res;
    }

}