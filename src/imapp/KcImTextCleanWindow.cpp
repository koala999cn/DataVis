#include "KcImTextCleanWindow.h"
#include "KuPathUtil.h"
#include "imgui.h"
#include "KuStrUtil.h"
#include "KuMatrixUtil.h"


namespace kPrivate
{
    enum // 遇到非数字token时的处理方式
    {
        k_illegal_fail,
        k_illegal_ignore,
        k_illegal_as_empty,
        k_illegal_as_nan,
        k_illegal_as_zero
    };

    enum // 遇到空token时的处理方式
    {
        k_empty_skip,
        k_empty_as_nan,
        k_empty_as_zero
    };
}

KcImTextCleanWindow::KcImTextCleanWindow(const std::string& source, matrix<std::string>& rawData)
    : KvImModalWindow(KuPathUtil::fileName(source))
    , source_(source)
    , rawData_(rawData)
{
    updateStats_();
}


void KcImTextCleanWindow::updateImpl_()
{
    assert(!rawData_.empty());

    /// draw stats
    ImGui::Text("Rows: %d", rows_);
    if (minCols_ == maxCols_)
        ImGui::Text("Columns: %d", minCols_);
    else 
        ImGui::Text("Columns: %d - %d", minCols_, maxCols_);
    ImGui::Text("Empty tokens: %d", emptyTokens_);
    ImGui::Text("Illegal tokens: %d", illegalTokens_);

    /// draw UIs

    static const char* empty_modes[] = {
        "Skip", "As Nan", "As Zero"
    };

    static const char* illegal_modes[] = {
        "Fail", "Ignore", "As Empty", "As Nan", "As Zero"
    };

    ImGui::PushItemWidth(99);
    if (emptyTokens_ > 0) {
        ImGui::Combo("Empty token", &emptyMode_, empty_modes, IM_ARRAYSIZE(empty_modes), IM_ARRAYSIZE(empty_modes));
        ImGui::SameLine();
    }

    if(illegalTokens_ > 0)
        ImGui::Combo("Illegal token", &illegalMode_, illegal_modes, IM_ARRAYSIZE(illegal_modes), IM_ARRAYSIZE(illegal_modes));
    
    if(minCols_ != maxCols_)
        ImGui::Checkbox("Force columns aligned", &forceAlign_);

    ImGui::PopItemWidth();

    ImGui::BeginDisabled(parseFailed_);
    if (ImGui::Button("OK", ImVec2(99, 0))) {
        ImGui::EndDisabled();
        clean_();
        close();
        return;
    }
    ImGui::SetItemDefaultFocus();
    ImGui::EndDisabled();

    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(99, 0))) {
        close();
        rawData_.clear();
        return;
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    /// draw the table

    const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
    static ImGuiTableFlags flags =
        ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY |
        ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter |
        ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable |
        ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
    static int freeze_cols = 1;
    static int freeze_rows = 1;

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

                if (!ImGui::TableSetColumnIndex(c) && c > 0)
                    continue;

                if (c == 0)
                    ImGui::Text(KuStrUtil::toString(r).c_str()); // 打印行号
                else if (c <= rawData_[r - 1].size()) {
                    auto& tok = rawData_[r - 1][c - 1];
                    bool illegalToken = !KuStrUtil::isFloat(tok);
                    if (illegalToken) // 突出显示非法子串
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));

                    ImGui::Text(tok.c_str()); // 打印内容

                    if (illegalToken)
                        ImGui::PopStyleColor();
                }
                else
                    break; // next row
            }
        }
        ImGui::EndTable();
    }
}


void KcImTextCleanWindow::updateStats_()
{
    rows_ = rawData_.size();
    emptyTokens_ = 0;
    illegalTokens_ = 0;
    minCols_ = maxCols_ = rawData_[0].size();

    for (unsigned i = 0; i < rawData_.size(); i++) {

        int cols = rawData_[i].size();
        for (unsigned j = 0; j < rawData_[i].size(); j++) {
            auto& tok = rawData_[i][j];
            if (tok.empty()) {
                emptyTokens_++;
                if (emptyMode_ == kPrivate::k_empty_skip)
                    cols--;
            }
            else if (!KuStrUtil::isFloat(tok)) {
                illegalTokens_++;
                if (skipIllegal_())
                    cols--;
            }
        }

        if (cols == 0)
            rows_--; // 空行不计数
        else if (cols < minCols_)
            minCols_ = cols;
        else if (cols > maxCols_)
            maxCols_ = cols;
    }

    parseFailed_ = (minCols_ != maxCols_ && !forceAlign_) ||
        (illegalTokens_ != 0 && illegalMode_ == kPrivate::k_illegal_fail);
}


bool KcImTextCleanWindow::skipIllegal_()
{
    using namespace kPrivate;
    return illegalMode_ == k_illegal_ignore ||
        (illegalMode_ == k_illegal_as_empty && emptyMode_ == k_empty_skip);
}


void KcImTextCleanWindow::clean_()
{
    using namespace kPrivate;

    assert(!parseFailed_);

    cleanData_.reserve(rows_);

    for (unsigned r = 0; r < rawData_.size(); r++) {
        std::vector<double> data;
        data.reserve(maxCols_);

        for (unsigned c = 0; c < rawData_[r].size(); c++) {
            auto& tok = rawData_[r][c];

            if (tok.empty()) {
                if (emptyMode_ == k_empty_skip)
                    continue;
                data.push_back(emptyValue_(tok));
            }
            else if (KuStrUtil::isFloat(tok)) {
                data.push_back(KuStrUtil::toDouble(tok.c_str()));
            }
            else { // illegal tokens
                assert(illegalMode_ != k_illegal_fail);

                if (skipIllegal_())
                    continue;
                data.push_back(illegalValue_(tok));
            }
        }

        if (!data.empty()) {
            assert(data.size() >= minCols_ && data.size() <= maxCols_);
            data.resize(maxCols_, 0);
            cleanData_.emplace_back(std::move(data));
        }
    }

    assert(cleanData_.size() == rows_);
}


double KcImTextCleanWindow::emptyValue_(const std::string& tok)
{
    return emptyMode_ == kPrivate::k_empty_as_zero ? 0 : KtuMath<double>::nan;
}


double KcImTextCleanWindow::illegalValue_(const std::string& tok)
{
    return illegalMode_ == kPrivate::k_illegal_as_zero ? 0
        : illegalMode_ == kPrivate::k_illegal_as_nan ?
        KtuMath<double>::nan : emptyValue_(tok);
}

