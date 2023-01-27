#include "imguix.h"
#include "KuStrUtil.h"
#include "KuDataUtil.h"
#include "KvDiscreted.h"
#include "KcSampled2d.h"
#include "layout/KeAlignment.h"
#include "misc/cpp/imgui_stdlib.h"
#include "plot/KpContext.h"


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

    static void switchAlign(KeAlignment& curAlign, int newAlign)
    {
        if (curAlign.location() == newAlign)
            curAlign.toggleAlginFirst();
        else
            curAlign.setLocation(newAlign);

        if (curAlign & KeAlignment::k_hcenter)
            curAlign.setAlignFirst(KeAlignment::k_vert_first);
        else if (curAlign & KeAlignment::k_vcenter)
            curAlign.setAlignFirst(KeAlignment::k_horz_first);
    }

    static bool prefixCheckbox(const char* label, bool* v)
    {
        auto w = ImGui::CalcItemWidth();

        bool res = ImGui::Checkbox(label, v);

        ImGui::PushItemWidth(w - ImGui::GetItemRectSize().x - ImGui::GetStyle().ItemSpacing.x);
        ImGui::SameLine();

        return res;
    }

}


namespace ImGuiX
{
    using namespace ImGui;

	void addLineDashed(const ImVec2& a, const ImVec2& b, ImU32 col, float thickness, unsigned int num_segments, unsigned int on_segments, unsigned int off_segments)
	{
        if ((col >> 24) == 0)
            return;

        auto dl = GetWindowDrawList();
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
        Dummy(ImVec2(colors.size() * step - blockSpace, blockSize.y));
        auto blockMin = startPos;
        for (unsigned i = 0; i < colors.size(); i++) {
            auto blockMax = blockMin + blockSize;
            GetWindowDrawList()->AddRectFilled(
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
                        Text("dy");
                    else 
                        Text("%d", r);
                }
                else {
                    Text("%d", r + 1);
                }
            }
            else {
                Text("%g", fn(r, c - 1));
            }
        };

        showLargeTable(rows, cols + 1, fnShow, 1, 1, headers);
    }


    void showLargeTable(unsigned rows, unsigned cols, 
        std::function<void(unsigned, unsigned)> fnShow, 
        unsigned freezeCols, unsigned freeszRows,
        const std::vector<std::string>& headers)
    {
        const float TEXT_BASE_WIDTH = CalcTextSize("ABC").x;
        const float TEXT_BASE_HEIGHT = GetTextLineHeightWithSpacing();
        const static ImGuiTableFlags flags =
            ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY |
            ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter |
            ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable;

        if (cols > 64)
            cols = 64; // TODO: ImGui要求总列数不超过64

        if (BeginTable("LargeTable", cols, flags)) {

            TableSetupScrollFreeze(freezeCols, freeszRows);

            if (!headers.empty()) {
                for (unsigned c = 0; c < cols; c++)
                    TableSetupColumn(headers[c].c_str());
                TableHeadersRow();
            }

            ImGuiListClipper clipper;
            clipper.Begin(rows);
            while (clipper.Step()) {
                for (int r = clipper.DisplayStart; r < clipper.DisplayEnd; r++) {
                    TableNextRow();
                    for (int c = 0; c < cols; c++) 
                        if (TableSetColumnIndex(c))
                            fnShow(r, c);
                }
            }

            EndTable();
        }
    }

    bool treePush(const char* label, bool defaultOpen)
    {
        return TreeNodeEx(label, 
            defaultOpen ? ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding
                        : ImGuiTreeNodeFlags_FramePadding);
    }

    void treePop()
    {
        TreePop();
    }

    bool cbTreePush(const char* label, bool* show, bool* open)
    {
        std::string node("##N"); node += label;
        *open = treePush(node.c_str(), *open);
        SameLine();
        return Checkbox(label, show);
    }

    void cbTreePop()
    {
        TreePop();
    }

    bool cbInputText(const char* label, bool* show, std::string* text)
    {
        std::string box("##I"); box += label;
        bool res = kPrivate::prefixCheckbox(box.c_str(), show);
        res |= InputText(label, text);
        PopItemWidth();
        return res;
    }

    bool cbiTreePush(const char* label, bool* show, std::string* text, bool* open)
    {
        std::string node("##N"); node += label;
        *open = treePush(node.c_str(), *open);
        SameLine();
        return cbInputText(label, show, text);
    }

    void cbiTreePop() { cbTreePop(); }

    bool alignment(const char* label, KeAlignment& loc, bool defaultOpen)
    {
        if (treePush(label, defaultOpen)) {

            ImVec2 itemSize(CalcTextSize("Outter").x * 2, 0);

            bool outter = loc.outter();
            int spacing = 12;

            if (Button(loc & KeAlignment::k_vert_first ? "NW" : "WN", itemSize)) 
                kPrivate::switchAlign(loc, KeAlignment::k_left | KeAlignment::k_top);
            SameLine(0, spacing);

            if (Button("N", itemSize)) 
                kPrivate::switchAlign(loc, KeAlignment::k_top | KeAlignment::k_hcenter);
            SameLine(0, spacing);

            if (Button(loc & KeAlignment::k_vert_first ? "NE" : "EN", itemSize))
                kPrivate::switchAlign(loc, KeAlignment::k_right | KeAlignment::k_top);

            if (Button("W", itemSize)) 
                kPrivate::switchAlign(loc, KeAlignment::k_left | KeAlignment::k_vcenter);
            SameLine(0, spacing);

            if (Button(outter ? "Out" : "In", itemSize))
                loc.toggleSide();
            SameLine(0, spacing);

            if (Button("E", itemSize))
                kPrivate::switchAlign(loc, KeAlignment::k_right | KeAlignment::k_vcenter);

            if (Button(loc & KeAlignment::k_vert_first ? "SW" : "WS", itemSize))
                kPrivate::switchAlign(loc, KeAlignment::k_left | KeAlignment::k_bottom);
            SameLine(0, spacing);

            if (Button("S", itemSize))
                kPrivate::switchAlign(loc, KeAlignment::k_bottom | KeAlignment::k_hcenter);
            SameLine(0, spacing);

            if (Button(loc & KeAlignment::k_vert_first ? "SE" : "ES", itemSize))
                kPrivate::switchAlign(loc, KeAlignment::k_right | KeAlignment::k_bottom);

            treePop();

            return true;
        }

        return false;
    }


    bool penStyle(const char* label, int& style)
    {
        static const char* styleStr[] = {
            "none", "solid", "dot", "dash",
            "dash4", "dash8", "dash dot", "dash dot dot"
        };
        return Combo(label, &style, styleStr, std::size(styleStr));
    }


    bool pen(KpPen& cxt, bool showStyle)
    {
        PushID(&cxt);

        bool res = false;
        
        if (showStyle) 
            res |= penStyle("Style", cxt.style);

        res |= SliderFloat("Width", &cxt.width, 0.1, 5.0, "%.1f px");
        res |= ColorEdit4("Color", cxt.color);
  
        PopID();

        return res;
    }


    bool brush(KpBrush& cxt, bool showStyle)
    {
        PushID(&cxt);

        bool res = false;

        if (showStyle) {
            static const char* styleStr[] = {
                "none", "solid"
            };
            res |= Combo("Style", &cxt.style, styleStr, std::size(styleStr));
        }

        res |= ColorEdit4("Color", cxt.color);

        PopID();

        return res;
    }

    template<typename T>
    bool margins_(const char* label, KtMargins<T>& m)
    {
        BeginGroup();
        PushID(&m);

        const ImGuiStyle& style = ImGui::GetStyle();
        const float w_full = CalcItemWidth();
        const float w_inputs = w_full;
        constexpr int components = 4;
        const float w_item_one = std::max(1.0f, std::floorf((w_inputs - (style.ItemInnerSpacing.x) * (components - 1)) / (float)components));
        const float w_item_last = std::max(1.0f, std::floorf(w_inputs - (w_item_one + style.ItemInnerSpacing.x) * (components - 1)));

        bool value_changed = false;
        static const char* ids[4] = { "##L", "##T", "##R", "##B" };
        static const char* fmt_table_[4] = {
            "L:%.f px", "T:%.f px", "R:%.f px", "B:%.f px"
        };
        static const T min_ = 0.;
        static const T max_ = 30.;

        int dataType;
        if constexpr (std::is_same_v<T, float>)
            dataType = ImGuiDataType_Float;
        else
            dataType = ImGuiDataType_Double;

        for (int n = 0; n < components; n++) {
            SetNextItemWidth((n + 1 < components) ? w_item_one : w_item_last);
            value_changed |= DragScalar(ids[n], dataType, &m[n], 1.0f, &min_, &max_, fmt_table_[n]);
            SameLine(0, style.ItemInnerSpacing.x);
        }

        SameLine(0.0f, style.ItemInnerSpacing.x);
        Text(label);

        PopID();
        EndGroup();

        return value_changed;
    }

    bool margins(const char* label, KtMargins<float>& m)
    {
        return margins_<float>(label, m);
    }


    bool margins(const char* label, KtMargins<double>& m)
    {
        return margins_<double>(label, m);
    }

    bool markerType(const char* label, int& type)
    {
        static const char* typeStr[] = {
            "dot",
            // "ball",  
            "circle",
            "square",
            "diamond",
            "up",
            "down",
            "left",
            "right",
            "cross",
            "plus",
            "asterisk"
        };

        return Combo(label, &type, typeStr, std::size(typeStr));
    }


    bool marker(KpMarker& cxt)
    {
        PushID(&cxt);
        bool res = false;

        res |= markerType("Scatter", cxt.type);
        res |= DragFloat("Size", &cxt.size, 0.1, 0.1, 10, "%.1f");
        res |= ColorEdit4("Color##Fill", cxt.fill);
        if (cxt.hasOutline()) {
            bool open(false);
            res |= cbTreePush("Outline", &cxt.showOutline, &open);
            if (open) {
                res |= DragFloat("Weight", &cxt.weight, 0.1, 0.1, 5, "%.1f px");
                res |= ColorEdit4("Color##Outline", cxt.outline);
                cbTreePop();
            }
        }

        PopID();
        return res;
    }


    bool gradient(const char* label, KtGradient<float, color4f>& grad)
    {
        return false;
    }
}