#include "KcImTextCleaner.h"
#include "KuPathUtil.h"
#include "imgui.h"
#include "KuStrUtil.h"
#include "KuMatrixUtil.h"
#include "imguix.h"

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

    static std::string rexpNA = "na|nan|n/a"; // TODO:
}


namespace kPrivate
{
    std::string localToUtf8(const std::string& str);
}

KcImTextCleaner::KcImTextCleaner(const std::string& source, const matrix<std::string_view>& rawData, matrix<double>& cleanData)
    : KvImModalWindow(kPrivate::localToUtf8(KuPathUtil::fileName(source)))
    , source_(source)
    , rawData_(rawData)
    , cleanData_(cleanData)
{
    updateStats_();
    if (emptyTokens_ == 0 && illegalTokens_ == 0 && !parseFailed_) {
        // 数据正常，没有需要用户配置的，直接清洗数据并关闭窗口
        setVisible(false);
        clean_();
    }
}


void KcImTextCleaner::updateImpl_()
{
    assert(!rawData_.empty());
    assert(visible());

    /// draw stats
    ImGui::Text("Rows: %d", rows_);
    if (minCols_ == maxCols_)
        ImGui::Text("Columns: %d", maxCols_);
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
        if (ImGui::Combo("Empty token", &emptyMode_, empty_modes, 
            IM_ARRAYSIZE(empty_modes), IM_ARRAYSIZE(empty_modes)))
            updateStats_();
        ImGui::SameLine();
    }

    if(illegalTokens_ > 0)
        if (ImGui::Combo("Illegal token", &illegalMode_, illegal_modes, 
            IM_ARRAYSIZE(illegal_modes), IM_ARRAYSIZE(illegal_modes)))
            updateStats_();
    
    if(minCols_ != maxCols_)
        ImGui::Checkbox("Force columns aligned", &forceAlign_);

    ImGui::PopItemWidth();

    ImGui::BeginDisabled(parseFailed_);
    if (ImGui::Button("OK", ImVec2(99, 0))) {
        close();
        clean_();
    }
    ImGui::SetItemDefaultFocus();
    ImGui::EndDisabled();

    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(99, 0))) {
        close();
        cleanData_.clear();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    /// draw the table

    showTable_();
}


void KcImTextCleaner::updateStats_()
{
    emptyLines_.clear();
    rows_ = rawData_.size();
    emptyTokens_ = 0;
    illegalTokens_ = 0;
    minCols_ = maxCols_ = rawData_[0].size();

    for (unsigned i = 0; i < rawData_.size(); i++) {

        int cols = rawData_[i].size();
        if (cols < minCols_)
            minCols_ = cols;
        else if (cols > maxCols_)
            maxCols_ = cols;

        for (unsigned j = 0; j < rawData_[i].size(); j++) {
            auto& tok = rawData_[i][j];
            if (tok.empty()) {
                emptyTokens_++;
                //if (emptyMode_ == kPrivate::k_empty_skip)
                //    cols--;
            }
            else if (!KuStrUtil::isFloat(tok)) {
                illegalTokens_++;
                //if (skipIllegal_())
                //    cols--;
            }
        }

        //if (cols == 0) {
        //    emptyLines_.insert(i);
        //    rows_--; // 空行不计数
       // }
    }

    parseFailed_ = (minCols_ != maxCols_ && !forceAlign_) ||
        (illegalTokens_ != 0 && illegalMode_ == kPrivate::k_illegal_fail);
}


bool KcImTextCleaner::skipIllegal_() const
{
    using namespace kPrivate;
    return illegalMode_ == k_illegal_ignore ||
        (illegalMode_ == k_illegal_as_empty && emptyMode_ == k_empty_skip);
}


void KcImTextCleaner::clean_()
{
    using namespace kPrivate;

    assert(!parseFailed_);

    cleanData_.clear();
    cleanData_.reserve(rows_);

    for (unsigned r = 0; r < rawData_.size(); r++) {
        std::vector<double> data;
        data.reserve(maxCols_);

        for (unsigned c = 0; c < rawData_[r].size(); c++) {
            auto& tok = rawData_[r][c];

            if (tok.empty()) {
                if (emptyMode_ == k_empty_skip)
                    continue;
                data.push_back(emptyValue_());
            }
            else if (KuStrUtil::isFloat(tok)) {
                data.push_back(KuStrUtil::toDouble(std::string(tok).c_str()));
            }
            else { // illegal tokens
                assert(illegalMode_ != k_illegal_fail);

                if (skipIllegal_())
                    continue;
                data.push_back(illegalValue_());
            }
        }

        if (!data.empty())
            cleanData_.emplace_back(std::move(data));
    }

    auto cr = KuMatrixUtil::colsRange(cleanData_);
    if (cr.first != cr.second) {
        assert(forceAlign_);
        for (auto& row : cleanData_)
            row.resize(cr.second, 0);
    }

    // 若有"ignore模式"造成的空行，该断言不成立
    // assert(cleanData_.size() == rows_);
}


double KcImTextCleaner::emptyValue_() const
{
    return emptyMode_ == kPrivate::k_empty_as_zero ? 0 : KuMath::nan<double>();
}


double KcImTextCleaner::illegalValue_() const
{
    return illegalMode_ == kPrivate::k_illegal_as_zero ? 0
        : illegalMode_ == kPrivate::k_illegal_as_nan ?
        KuMath::nan<double>() : emptyValue_();
}


void KcImTextCleaner::showTable_() const
{
    std::vector<std::string> headers(std::min(maxCols_ + 1, 64));
    headers[0] = "NO.";
    for (unsigned i = 1; i < headers.size(); i++)
        headers[i] = KuStrUtil::toString(i);
        
    auto fnShow = [&headers, this](unsigned r, unsigned c) {
        if (c == 0) { // show the row-index
            ImGui::Text("%d", r + 1);
        }
        else if (c <= rawData_[r].size()) {
            auto& tok = rawData_[r][c - 1];
            if (tok.empty())
                ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "EMPTY");
            else if (!KuStrUtil::isFloat(tok)) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
                ImGui::TextUnformatted(tok.data(), tok.data() + tok.size());
                ImGui::PopStyleColor();
            }
            else
                ImGui::TextUnformatted(tok.data(), tok.data() + tok.size());
        }
        else 
            ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "---");
    };
    
    ImGuiX::showLargeTable(rawData_.size(), maxCols_ + 1, fnShow, 1, 1, headers);
}
