#include "imguix.h"
#include "imgui_internal.h"
#include "dsp/KuDataUtil.h"
#include "dsp/KvDiscreted.h"
#include "dsp/KcSampled2d.h"
#include "layout/KeAlignment.h"
#include "misc/cpp/imgui_stdlib.h"
#include "plot/KpContext.h"
#include "util/draw_gradient.h"
#include "imapp/KcImExprEditor.h"
#include "imapp/KsImApp.h"
#include "imapp/KgImWindowManager.h"
#include "layout/KcLayoutGrid.h"
#include "layout/KcLayoutOverlay.h"

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
                headers.push_back(str + std::to_string(c));
        }
        else if (dataType == KuDataUtil::k_scattered_1d) {
            for (int ch = 1; ch <= cols / 2; ch++) {
                auto chIdx = std::to_string(ch);
                headers.push_back("X" + chIdx);
                headers.push_back("Y" + chIdx);
            }
        }
        else if (dataType == KuDataUtil::k_scattered_2d) {
            for (int ch = 1; ch <= cols / 3; ch++) {
                auto chIdx = std::to_string(ch);
                headers.push_back("X" + chIdx);
                headers.push_back("Y" + chIdx);
                headers.push_back("Z" + chIdx);
            }
        }
        else if (dataType == KuDataUtil::k_series) {
            for (int c = 0; c < cols; c++)
                headers.push_back(seriesName_(c));
        }
        else if (dataType == KuDataUtil::k_sampled_2d) {
            headers.push_back("dx");
            for (int c = 1; c <= cols; c++)
                headers.push_back(std::to_string(c));
        }
        else {
            for (int c = 1; c <= cols; c++)
                headers.push_back(std::to_string(c));
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

        ImGui::PushItemWidth(w - ImGui::GetItemRectSize().x - ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

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
        static std::vector<char> vis;
        showDataTable(data, vis);
    }

    void showDataTable(const KvData& data, std::vector<char>& vis)
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
                    }, vis);
            }
            else if (disc.isSampled()) {
                if (disc.dim() == 1) {
                    if (disc.step(0) == 1) { // series 
                        showDataTable(KuDataUtil::k_series, disc.size(), disc.channels(),
                            [&disc](unsigned r, unsigned c) {
                                return disc.pointAt(r, c).at(1);
                            }, vis);
                    }
                    else { // sampled1d
                        showDataTable(KuDataUtil::k_sampled_1d, disc.size(), disc.channels() + 1,
                            [&disc](unsigned r, unsigned c) {
                                if (c == 0)
                                    return disc.pointAt(r, 0).at(0);
                                else
                                    return disc.valueAt(r, c - 1);
                            }, vis);
                    }
                }
                else if (disc.dim() == 2) {
                    auto samp2d = dynamic_cast<const KvSampled*>(&disc);
                    if (disc.step(0) == 1 && disc.step(1) == 1) { // matrix
                        showDataTable(KuDataUtil::k_matrix, disc.size(1), disc.size(0),
                            [samp2d](unsigned r, unsigned c) {
                                return samp2d->value(c, r, 0);
                            }, vis);
                    }
                    else { // sampled2d
                        showDataTable(KuDataUtil::k_sampled_2d, disc.size(1) + 1, disc.size(0) + 1,
                            [samp2d](unsigned r, unsigned c) -> double {
                                if (r == 0 && c == 0)
                                    return std::numeric_limits<double>::quiet_NaN();
                                else if (r == 0)
                                    return samp2d->range(0).low() + (c - 1) * samp2d->step(0);
                                else if (c == 0)
                                    return samp2d->range(1).low() + (r - 1) * samp2d->step(1);
                                else 
                                    return samp2d->value(c - 1, r - 1, 0);
                            }, vis);
                    }
                }
                
            }
        }
    }


    void showDataTable(int type, const matrixd& data, bool rowMajor, std::vector<char>& vis)
    {
        if (data.empty())
            return;

        unsigned rows = data.size();
        unsigned cols = data[0].size();
        if (rowMajor)
            std::swap(rows, cols);
        vis.resize(cols);

        showDataTable(type, rows, cols, [&data, rowMajor](unsigned r, unsigned c) {
            return rowMajor ? data[c][r] : data[r][c]; }, vis);
    }


    void showDataTable(int type, unsigned rows, unsigned cols, std::function<double(unsigned, unsigned)> fn,
        std::vector<char>& vis)
    {
        auto headers = kPrivate::makeTableHeaders_(type, std::min(cols, 510u)); // 510为2与3的公倍数
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

        vis.resize(cols + 1);
        showLargeTable(rows, cols + 1, fnShow, 1, 1, headers, vis.data());
    }


    void showLargeTable(unsigned rows, unsigned cols, 
        std::function<void(unsigned, unsigned)> fnShow,
        unsigned freezeCols, unsigned freeszRows,
        const std::vector<std::string>& headers, char* vis)
    {
        const float TEXT_BASE_WIDTH = CalcTextSize("ABC").x;
        const float TEXT_BASE_HEIGHT = GetTextLineHeightWithSpacing();
        const static ImGuiTableFlags flags = ImGuiTableFlags_RowBg |
            ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY |
            ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV |
            ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp |
            ImGuiTableFlags_Hideable | ImGuiTableFlags_ContextMenuInBody;

        if (cols > 512)
            cols = 512; // NB: ImGui(v1.89.4)要求总列数不超过512

        if (BeginTable("LargeTable", cols, flags)) {

            TableSetupScrollFreeze(freezeCols, freeszRows);

            if (!headers.empty()) {
                for (unsigned c = 0; c < cols; c++)
                    TableSetupColumn(headers[c].c_str(), c >= freezeCols ? 0 : ImGuiTableColumnFlags_NoHide);
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

            if (vis) {
                for (unsigned c = 0; c < cols; c++)
                    vis[c] = TableGetColumnFlags(c) & ImGuiTableColumnFlags_IsEnabled;
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
        SameLine(0, GetStyle().ItemSpacing.x);
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
        SameLine(0, GetStyle().ItemSpacing.x);
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


    bool pen(KpPen& cxt, bool showStyle, bool showColor)
    {
        PushID(&cxt);

        bool res = false;
        
        if (showStyle) 
            res |= penStyle("Style", cxt.style);

        res |= SliderFloat("Width", &cxt.width, 0.1, 5.0, "%.1f px");

        if (showColor)
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
    bool margins_(const char* label, KtAABB<T, 2>& m)
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

        T* p = (T*)&m;
        for (int n = 0; n < components; n++) {
            SetNextItemWidth((n + 1 < components) ? w_item_one : w_item_last);
            value_changed |= DragScalar(ids[n], dataType, p + n, 1.0f, &min_, &max_, fmt_table_[n]);
            SameLine(0, style.ItemInnerSpacing.x);
        }

        SameLine(0.0f, style.ItemInnerSpacing.x);
        Text(label);

        PopID();
        EndGroup();

        return value_changed;
    }

    bool margins(const char* label, KtAABB<float, 2>& m)
    {
        return margins_<float>(label, m);
    }


    bool margins(const char* label, KtAABB<double, 2>& m)
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

        res |= markerType("Type", cxt.type);
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


    bool gradient(const char* label, KtGradient<float, color4f>& grad, float& selectedKey)
    {   
        auto drawList = GetWindowDrawList();
        auto offset = GetCursorScreenPos();
        auto size = ImVec2(CalcItemWidth(), GetFrameHeight());
        auto barHeight = GetTextLineHeight();
        auto fillQuad = [drawList](KtPoint<float, 2>* vtx, color4f* clrs) {
            drawList->PrimReserve(6, 6);

            auto uv = drawList->_Data->TexUvWhitePixel;
            drawList->PrimVtx((ImVec2&)vtx[0], uv, ImColor((ImVec4&)clrs[0]));
            drawList->PrimVtx((ImVec2&)vtx[1], uv, ImColor((ImVec4&)clrs[1]));
            drawList->PrimVtx((ImVec2&)vtx[2], uv, ImColor((ImVec4&)clrs[2]));
            drawList->PrimVtx((ImVec2&)vtx[0], uv, ImColor((ImVec4&)clrs[0]));
            drawList->PrimVtx((ImVec2&)vtx[2], uv, ImColor((ImVec4&)clrs[2]));
            drawList->PrimVtx((ImVec2&)vtx[3], uv, ImColor((ImVec4&)clrs[3]));
        };

        auto w = size.y - barHeight + 2; // 按钮宽度

        // 绘制背景
        Dummy(size);
        //drawList->AddRectFilled(offset, offset + size, ImColor(GetStyle().Colors[ImGuiCol_Button]));

        // 绘制色带
        auto x0 = offset.x + w / 2;
        auto x1 = offset.x + size.x - w / 2;
        auto y0 = offset.y + w / 2 - 1;
        auto y1 = offset.y + barHeight + 2;
        drawGradient<float, float>(fillQuad, { x0, y0 }, { x1, y1 }, grad, 0);
       
        // 绘制控制点
        auto ymax = offset.y + size.y;
        static bool dragging(false); // 是否正拖动控制点
        auto& style = GetStyle();
        for (auto& i : grad) {
            auto x = KuMath::remap(i.first, 0.f, 1.f, x0, x1); // 定位控制点的水平坐标. TOOD: 此处默认grad已规范化

            ImVec2 ptmin(x - w / 2, offset.y), ptmax(x + w / 2, ymax);
            bool hovering = IsMouseHoveringRect(ptmin, ptmax);
            bool selecting = (selectedKey == i.first);

            auto fill = i.second; 
            if (selecting);
            else if (hovering) fill.w() *= 0.9;
            else fill.w() *= 0.8;

            auto border = selecting ? ImGuiCol_FrameBgActive : hovering ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg;
            auto thickness = selecting ? 2.5 : hovering ? 2 : 1;
            drawList->AddRectFilled(ptmin, ptmax, ImColor((ImVec4&)fill), 4);
            drawList->AddRect(ptmin, ptmax, ImColor(style.Colors[border]), 4, 0, thickness);

            if (hovering && !dragging) {
                if (IsMouseDown(ImGuiMouseButton_Left)) {
                    selectedKey = i.first;
                    dragging = true;
                }
                else if (IsMouseDown(ImGuiMouseButton_Right)) {
                    OpenPopup("picker");
                }
            }
        }

        SameLine(0.0f, style.ItemInnerSpacing.x);
        Text(label);

        bool value_changed(false); // NB: selectedKey变化不影响changed值

        // 移动关键点
        if (IsMouseReleased(ImGuiMouseButton_Left)) 
            dragging = false;

        if (dragging) { 

            // 以下代码捕获输入，防止拖动窗口（否则拖动效果不好，很多时候会移动窗口）
            ImGuiWindow* window = GetCurrentWindow();
            const ImGuiID id = window->GetID(label);
            SetActiveID(id, window);
            SetFocusID(id, window);
            FocusWindow(window);

            if (IsMouseDragging(ImGuiMouseButton_Left)) {
                auto newKey = KuMath::remap<float, true>(GetMousePos().x, x0, x1);
                if (!grad.has(newKey)) { // 防止拖动的时候吃掉其他控制点
                    grad.move(selectedKey, newKey);
                    selectedKey = newKey;
                    value_changed = true;
                }
            }
        }

        // 删除关键点
        if (IsKeyPressed(ImGuiKey_Delete) 
            && IsWindowFocused()
            && grad.has(selectedKey)
            && grad.size() > 2) { // 超过2个控制点允许删除
            grad.erase(selectedKey);
            selectedKey = KuMath::nan<float>();
            value_changed = true;
        }
        
        // 编辑控制点
        if (BeginPopup("picker")) {
            auto val = grad.map(selectedKey);
            auto refval = val;
            if (ColorPicker4("##picker", val, 0, refval)) {
                grad.insert(selectedKey, val);
                value_changed = true;
            }

            EndPopup();
        }
        else 
        // 显示tooltip & 新增关键点
        if (IsMouseHoveringRect(offset, offset + size)) {
            auto key = KuMath::remap<float, true>(GetMousePos().x, x0, x1);
            auto val = grad.map(key);
            auto loc = "Location:" + std::to_string(key);
            ColorTooltip(loc.c_str(), val, ImGuiColorEditFlags_AlphaPreview);

            if (IsMouseDoubleClicked(ImGuiMouseButton_Left))
                grad.insert(key, val);
        }

        return value_changed;
    }


    bool multiColorsEdit(const char* label, ImVec4* clrs, int count, bool fullfill)
    {
        bool value_changed = false;
        const auto flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel;
        auto spacing = GetStyle().ItemInnerSpacing.x;

        if (fullfill) {
            auto fullw = CalcItemWidth();    
            auto w = (fullw - spacing * (count - 1)) / count;

            for (int i = 0; i < count; i++) {

                PushID(i);

                // NB: 由于ColorEdit*不能调整宽度，此处使用ColorButton
                if (ColorButton("##", clrs[i], flags, ImVec2(w, 0)))
                    OpenPopup("picker");

                if (ImGui::BeginPopup("picker")) {
                    value_changed |= ImGui::ColorPicker4("##picker", &clrs[i].x);
                    EndPopup();
                }

                PopID();

                if (i != count - 1)
                    SameLine(0.0f, spacing);
            }
        }
        else {
            for (int i = 0; i < count; i++) {
                
                PushID(i);
                value_changed |= ColorEdit4("##", &clrs[i].x, flags);
                PopID();

                if (i != count - 1)
                    SameLine(0.0f, spacing);
            }
        }

        if (label) {
            SameLine(0, GetStyle().ItemInnerSpacing.x);
            Text(label);
        }

        return value_changed;
    }


    void exprEdit(const char* label, const char* text, unsigned dim, 
        std::function<void(std::shared_ptr<KvData>, const char*)> handler)
    {
        auto w = CalcItemWidth();
        auto h = GetFrameHeight();

        if (ImGui::Button("E", ImVec2(h, h))) { // 编辑表达式字符串
            KsImApp::singleton().windowManager().
                registerWindow<KcImExprEditor>(text, dim, handler);
        }

        ImGui::PushItemWidth(w - h - ImGui::GetStyle().ItemSpacing.x);
        ImGui::SameLine();

        ImGui::BeginDisabled();
        std::string str = text;
        ImGui::InputText(label, &str, ImGuiInputTextFlags_ReadOnly);
        ImGui::EndDisabled();

        ImGui::PopItemWidth();
    }


    const char* layLabel_(KvLayoutElement* lay) 
    {
        if (dynamic_cast<KcLayoutGrid*>(lay))
            return "LayoutGrid";
        else if (dynamic_cast<KcLayoutOverlay*>(lay))
            return "LayoutOverlay";
        else if (dynamic_cast<KcLayout2d*>(lay))
            return "Layout2d";
        else if (dynamic_cast<KcLayout1d*>(lay))
            return "Layout1d";
        return "Element";
    }

    void layout(const char* label, KvLayoutElement* lay)
    {
        assert(lay);

        PushID(lay);

        if (treePush(label, false)) {

            auto or = lay->outterRect();
            margins("Outter Rect", or);

            auto ir = lay->innerRect();
            margins("Inner Rect", ir);

            margins("Margins", lay->margins());

            auto sc = lay->contentSize();
            ImGui::DragScalarN("Content Size", ImGuiDataType_Double, &sc, 2);

            auto sr = lay->expectRoom();
            ImGui::DragScalarN("Expect Room", ImGuiDataType_Double, &sr, 2);

            auto shares = lay->extraShares();
            ImGui::DragInt2("Extra Shares", (int*)&shares);

            LabelText("Alignment", lay->align().format().c_str());

            if (dynamic_cast<KvLayoutContainer*>(lay)) {
                if (treePush("Children", false)) {
                    auto c = dynamic_cast<KvLayoutContainer*>(lay);
                    for (auto& e : c->elements()) 
                        if (e) layout(layLabel_(e.get()), e.get());
                    treePop();
                }
            }

            treePop();
        }


        PopID();
    }
}