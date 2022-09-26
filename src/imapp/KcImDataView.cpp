#include "KcImDataView.h"
#include "KuPathUtil.h"
#include "imgui.h"
#include "KuStrUtil.h"
#include "KuMatrixUtil.h"


KcImDataView::KcImDataView(const std::string& source, matrix<std::string>& rawData)
    : KvImModalWindow(KuPathUtil::fileName(source))
    , source_(source)
    , rawData_(rawData)
{

}


void KcImDataView::updateImpl_()
{
    const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
    static ImGuiTableFlags flags = 
        ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY | 
        ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | 
        ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | 
        ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
    static int freeze_cols = 1;
    static int freeze_rows = 1;

    bool keepEmpty = false;
    ImGui::Checkbox("keep empty tokens", &keepEmpty);

    if (ImGui::Button("OK", ImVec2(120, 0))) {
        close();
        return;
    }
    ImGui::SetItemDefaultFocus();
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(120, 0))) {
        close();
        rawData_.clear();
        return;
    }
    ImGui::Separator();

    assert(!rawData_.empty());
    int cols = KuMatrixUtil::colsRange(rawData_).second;
    if (cols >= 64)
        cols = 63; // TODO: ImGui要求总列数不超过64，此处首列留给行序号

    if (ImGui::BeginTable("table_scrollx", cols + 1, flags)) {

        ImGui::TableSetupScrollFreeze(freeze_cols, freeze_rows);
        ImGui::TableSetupColumn("No.", ImGuiTableColumnFlags_NoHide); // Make the first column not hideable to match our use of TableSetupScrollFreeze()
        for (int c = 1; c <= cols; c++)
            ImGui::TableSetupColumn(KuStrUtil::toString(c).c_str());
        ImGui::TableHeadersRow();

        for (int r = 1; r <= rawData_.size(); r++) {
            ImGui::TableNextRow();
            for (int c = 0; c <= cols; c++) {
                // Both TableNextColumn() and TableSetColumnIndex() return true when a column is visible or performing width measurement.
                // Because here we know that:
                // - A) all our columns are contributing the same to row height
                // - B) column 0 is always visible,
                // We only always submit this one column and can skip others.
                // More advanced per-column clipping behaviors may benefit from polling the status flags via TableGetColumnFlags().
                if (!ImGui::TableSetColumnIndex(c) && c > 0)
                    continue;

                if (c == 0)
                    ImGui::Text(KuStrUtil::toString(r).c_str()); // 打印行号
                else if (c <= rawData_[r - 1].size())
                    ImGui::Text(rawData_[r - 1][c - 1].c_str()); // 打印内容
                else
                    break; // next row
            }
        }
        ImGui::EndTable();
    }
}
