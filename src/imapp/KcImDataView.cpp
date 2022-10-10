#include "KcImDataView.h"
#include "KuPathUtil.h"
#include "imgui.h"
#include "KuMatrixUtil.h"
#include "KcSampled1d.h"
#include "KcSampled2d.h"
#include "KtScattered.h"
#include "KuStrUtil.h"


namespace kPrivate
{
    enum KeDataType
    {
        k_series,
        k_matrix,
        k_scattered_1d,
        k_scattered_2d,
        k_sampled_1d,
        k_sampled_2d
    };

    // 检测mat可转换的数据类型
    // 返回[0]为推荐类型
    static std::vector<int> validTypes_(const KcImDataView::matrixd& mat, bool colMajor)
    {
        if (mat.empty())
            return {};

        auto rows = mat.size();
        auto cols = mat[0].size();

        if (rows == 0 && cols == 0)
            return {};

        if (!colMajor)
            std::swap(rows, cols);

        std::vector<int> types;

        types.push_back(k_series);

        if (rows > 8 && cols > 8)
            types.insert(types.begin(), k_matrix);
        else
            types.push_back(k_matrix);

        if (cols > 1) {

            if (cols % 2 == 0)
                types.insert(types.begin(), k_scattered_1d);

            if (cols % 3 == 0)
                types.insert(types.begin(), k_scattered_2d);

            bool evenRow = KuMatrixUtil::isEvenlySpaced(mat[0]);
            bool evenCol = KuMatrixUtil::isEvenlySpaced(KuMatrixUtil::extractColumn(mat, 0));
            if (colMajor && evenCol || !colMajor && evenRow)
                types.insert(types.begin(), k_sampled_1d);

            if (rows > 1 && evenRow && evenCol)
                types.insert(types.begin(), k_sampled_2d);
        }

        return types;
    }

}


KcImDataView::KcImDataView(const std::string& source, const matrixd& idata, std::shared_ptr<KvData>& odata)
    : KvImModalWindow(KuPathUtil::fileName(source))
    , idata_(idata), odata_(odata)
{
    assert(!idata.empty());

    rowMajor_ = idata.size() < idata[0].size();
    typeList_ = kPrivate::validTypes_(idata, !rowMajor_);
    dataType_ = typeList_.front();
}


void KcImDataView::updateImpl_()
{
    ImGui::PushItemWidth(119);

    if (ImGui::Checkbox("Row major", &rowMajor_)) {
        typeList_ = kPrivate::validTypes_(idata_, !rowMajor_);
        dataType_ = typeList_.front();
    }

    static const std::pair<int, const char*> supportTypes[] = {
        { kPrivate::k_series,       "series"       },
        { kPrivate::k_matrix,       "matrix"       },
        { kPrivate::k_scattered_1d, "scattered-1d" },
        { kPrivate::k_scattered_2d, "scattered-2d" },
        { kPrivate::k_sampled_1d,   "sampled-1d"   },
        { kPrivate::k_sampled_2d,   "sampled-2d"   }
    };

    if (ImGui::BeginCombo("Data type", supportTypes[dataType_].second)) {
        for (int n = 0; n < typeList_.size(); n++)
            if (ImGui::Selectable(supportTypes[typeList_[n]].second, typeList_[n] == dataType_))
                dataType_ = typeList_[n];

        ImGui::EndCombo();
    }

    ImGui::PopItemWidth();


    if (ImGui::Button("OK", ImVec2(99, 0))) {
        close();

        // 生成数据
        odata_ = makeData_(rowMajor_ ? idata_ : KuMatrixUtil::transpose(idata_), dataType_);
    }
    ImGui::SetItemDefaultFocus();

    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(99, 0))) {
        close();
        odata_ = nullptr; // 标记为取消状态
    }

    showTable_();
}


std::shared_ptr<KvData> KcImDataView::makeSeries_(const matrixd& mat)
{
    // 暂时使用sampled1d表示series数据

    auto samp = std::make_shared<KcSampled1d>();
    samp->resize(mat[0].size(), mat.size());
    samp->reset(0, 0, 1);

    for (unsigned c = 0; c < samp->channels(); c++)
        samp->setChannel(0, c, mat[c].data());

    return samp;
}



std::shared_ptr<KvData> KcImDataView::makeMatrix_(const matrixd& mat)
{
    auto samp2d = std::make_shared<KcSampled2d>();
    samp2d->resize(mat.size(), mat[0].size());
    samp2d->reset(0, 0, 1);
    samp2d->reset(1, 0, 1);

    for (kIndex idx = 0; samp2d->size(0); idx++)
        samp2d->setChannel(&idx, 0, mat[idx].data()); // 始终单通道

    return samp2d;
}


std::shared_ptr<KvData> KcImDataView::makeSampled1d_(const matrixd& mat)
{
    auto samp = std::make_shared<KcSampled1d>();
    samp->resize(mat[0].size(), mat.size() - 1);
    samp->reset(0, mat[0][0], mat[0][1] - mat[0][0]);

    for (unsigned c = 0; c < samp->channels(); c++)
        samp->setChannel(0, c, mat[c + 1].data());

    return samp;
}


std::shared_ptr<KvData> KcImDataView::makeSampled2d_(const matrixd& mat)
{
    // mat[0][0]为占位符，无效

    auto dx = mat[2][0] - mat[1][0];
    auto dy = mat[0][2] - mat[0][1];

    auto samp2d = std::make_shared<KcSampled2d>();
    samp2d->resize(mat.size() - 1, mat[0].size() - 1);
    samp2d->reset(0, mat[1][0], dx);
    samp2d->reset(1, mat[0][1], dy);

    for (kIndex idx = 0; samp2d->size(0); idx++)
        samp2d->setChannel(&idx, 0, mat[idx + 1].data() + 1); // 始终单通道, 忽略mat数据的首行首列

    return samp2d;
}


std::shared_ptr<KvData> KcImDataView::makeScattered_(const matrixd& mat, unsigned dim)
{
    assert(dim == 2 || dim == 3);
    assert(mat[0].size() % dim == 0);

    kIndex chs = mat.size() / dim;
    if (dim == 2) {
        auto scattered = std::make_shared<KtScattered<1>>();
        scattered->resize(nullptr, chs);
        scattered->reserve(mat[0].size());

        for (unsigned i = 0; i < mat[0].size(); i++) {
            std::vector<typename KtScattered<1>::element_type> points;
            for (unsigned c = 0; c < chs; c++)
                points.push_back({ mat[c * 2][i], mat[c * 2 + 1][i] });

            scattered->pushBack(points.data());
        }

        return scattered;
    }
    else {
        auto scattered = std::make_shared<KtScattered<2>>();
        scattered->resize(nullptr, chs);
        scattered->reserve(mat[0].size());

        for (unsigned i = 0; i < mat[0].size(); i++) {
            std::vector<typename KtScattered<2>::element_type> points;
            for (unsigned c = 0; c < chs; c++)
                points.push_back({ mat[c * 3][i], mat[c * 3 + 1][i], mat[c * 3 + 2][i] });

            scattered->pushBack(points.data());
        }

        return scattered;
    }
}


std::shared_ptr<KvData> KcImDataView::makeData_(const matrixd& mat, int type)
{
    using namespace kPrivate;

    std::shared_ptr<KvData> data;

    switch (type)
    {
    case k_series:
        data = makeSeries_(mat);
        break;

    case k_matrix:
        data = makeMatrix_(mat);
        break;

    case k_scattered_1d:
        data = makeScattered_(mat, 2);
        break;

    case k_scattered_2d:
        data = makeScattered_(mat, 3);
        break;

    case k_sampled_1d:
        data = makeSampled1d_(mat);
        break;

    case k_sampled_2d:
        data = makeSampled2d_(mat);
        break;

    default:
        assert(false);
    }

    return data;
}


void KcImDataView::showTable_() const
{
    const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
    static ImGuiTableFlags flags =
        ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY |
        ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter |
        ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable;
    static int freeze_cols = 1;
    static int freeze_rows = 1;

    int rows = idata_.size();
    int cols = idata_[0].size();
    if (rowMajor_)
        std::swap(rows, cols);

    if (cols >= 64)
        cols = 63; // TODO: ImGui要求总列数不超过64，此处首列留给行序号

    if (ImGui::BeginTable("RawData", cols + 1, flags)) {

        ImGui::TableSetupScrollFreeze(freeze_cols, freeze_rows);
        ImGui::TableSetupColumn("No.", ImGuiTableColumnFlags_NoHide); // Make the first column not hideable to match our use of TableSetupScrollFreeze()
        setupTableHeader_(cols);
        ImGui::TableHeadersRow();

        for (int r = 0; r < rows; r++) {

            ImGui::TableNextRow();
            for (int c = 0; c <= cols; c++) {

                if (!ImGui::TableSetColumnIndex(c) && c > 0)
                    continue;

                if (c == 0)
                    ImGui::Text(KuStrUtil::toString(r + 1).c_str()); // 打印行号
                else {
                     auto val = rowMajor_ ? idata_[c - 1][r] : idata_[r][c - 1];
                     ImGui::Text(KuStrUtil::toString(val).c_str());
                }
            }
        }
        ImGui::EndTable();
    }
}


void KcImDataView::setupTableHeader_(int cols) const
{
    using namespace kPrivate;

    if (dataType_ == k_sampled_1d) {
        ImGui::TableSetupColumn("time");
        std::string str("ch-");
        for (int c = 1; c < cols; c++)
            ImGui::TableSetupColumn((str + KuStrUtil::toString(c)).c_str());
    }
    else if (dataType_ == k_scattered_1d) {
        for (int ch = 1; ch <= cols / 2; ch++) {
            auto chIdx = KuStrUtil::toString(ch);
            ImGui::TableSetupColumn(("X" + chIdx).c_str());
            ImGui::TableSetupColumn(("Y" + chIdx).c_str());
        }
    }
    else if (dataType_ == k_scattered_2d) {
        for (int ch = 1; ch <= cols / 3; ch++) {
            auto chIdx = KuStrUtil::toString(ch);
            ImGui::TableSetupColumn(("X" + chIdx).c_str());
            ImGui::TableSetupColumn(("Y" + chIdx).c_str());
            ImGui::TableSetupColumn(("Z" + chIdx).c_str());
        }
    }
    else if (dataType_ == k_series) {
        for (int c = 0; c < cols; c++)
            ImGui::TableSetupColumn(seriesName_(c).c_str());
    }
    else {
        for (int c = 1; c <= cols; c++)
            ImGui::TableSetupColumn(KuStrUtil::toString(c).c_str());
    }
}


std::string KcImDataView::seriesName_(int idx)
{
    std::string name;
    while(true) {
        name.insert(name.begin(), 'A' + idx % 26);
        idx /= 26;

        if (idx == 0)
            break;
        --idx;
    }
 
    return name;
}
