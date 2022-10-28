#include "KcImDataMaker.h"
#include "KuPathUtil.h"
#include "KuStrUtil.h"
#include "KuMatrixUtil.h"
#include "KuDataUtil.h"
#include "imguix.h"


KcImDataMaker::KcImDataMaker(const std::string& source, const matrixd& idata, std::shared_ptr<KvData>& odata)
    : KvImModalWindow(KuPathUtil::fileName(source))
    , idata_(idata), odata_(odata)
{
    assert(!idata.empty());

    rowMajor_ = idata.size() < idata[0].size();
    typeList_ = KuDataUtil::validTypes(idata, !rowMajor_);
    dataType_ = typeList_.front();
}


void KcImDataMaker::updateImpl_()
{
    ImGui::PushItemWidth(119);

    if (ImGui::Checkbox("Row major", &rowMajor_)) {
        typeList_ = KuDataUtil::validTypes(idata_, !rowMajor_);
        dataType_ = typeList_.front();
    }

    using namespace ImGuiX;
    static const std::pair<int, const char*> supportTypes[] = {
        { KuDataUtil::k_series,       "series"       },
        { KuDataUtil::k_matrix,       "matrix"       },
        { KuDataUtil::k_scattered_1d, "scattered-1d" },
        { KuDataUtil::k_scattered_2d, "scattered-2d" },
        { KuDataUtil::k_sampled_1d,   "sampled-1d"   },
        { KuDataUtil::k_sampled_2d,   "sampled-2d"   }
    };

    if (ImGui::BeginCombo("Data type", supportTypes[dataType_].second)) {
        for (int n = 0; n < typeList_.size(); n++)
            if (ImGui::Selectable(supportTypes[typeList_[n]].second, typeList_[n] == dataType_))
                dataType_ = typeList_[n];

        ImGui::EndCombo();
    }

    ImGui::PopItemWidth();


    if (ImGui::Button("OK", ImVec2(99, 0))) {
        setVisible(false);
        close_();

        // 生成数据
        odata_ = KuDataUtil::makeData(rowMajor_ ? idata_ : KuMatrixUtil::transpose(idata_), dataType_);
    }
    ImGui::SetItemDefaultFocus();

    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(99, 0))) {
        setVisible(false);
        close_();
        odata_ = nullptr; // 标记为取消状态
    }

    ImGuiX::showDataTable(dataType_, idata_, rowMajor_);
}
