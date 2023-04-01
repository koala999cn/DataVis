#include "KcImDataMaker.h"
#include "KuPathUtil.h"
#include "KuStrUtil.h"
#include "KuMatrixUtil.h"
#include "KuDataUtil.h"
#include "imguix.h"


namespace kPrivate
{
    std::string localToUtf8(const std::string& str);
}

KcImDataMaker::KcImDataMaker(const std::string& source, const matrixd& idata, std::shared_ptr<KvData>& odata)
    : KvImModalWindow(kPrivate::localToUtf8(KuPathUtil::fileName(source)))
    , idata_(idata), odata_(odata)
{
    assert(!idata.empty());

    typeList_ = KuDataUtil::validTypes(idata, transpose_);
    dataType_ = typeList_.front();
}


void KcImDataMaker::updateImpl_()
{
    ImGui::PushItemWidth(119);

    if (ImGui::Checkbox("Transpose", &transpose_)) {
        typeList_ = KuDataUtil::validTypes(idata_, transpose_);
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
        for (int n = 0; n < typeList_.size(); n++) {
            if (ImGui::Selectable(supportTypes[typeList_[n]].second, typeList_[n] == dataType_))
                dataType_ = typeList_[n];
        }

        ImGui::EndCombo();
    }

    ImGui::PopItemWidth();


    if (ImGui::Button("OK", ImVec2(99, 0))) {
        close();

        // 删除用户筛选列
        matrixd idata = transpose_ ? KuMatrixUtil::transpose(idata_) : idata_;
        unsigned cols = idata.front().size();
        assert(vis_.size() >= cols);
        auto vis = vis_.rbegin();
        for (unsigned i = cols - 1; i != -1; i--) {
            if (!*vis++)
                KuMatrixUtil::eraseColumn(idata, i);
        }

        // 生成数据
        odata_ = KuDataUtil::makeData(idata, dataType_);
    }
    ImGui::SetItemDefaultFocus();

    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(99, 0))) {
        close();
        odata_ = nullptr; // 标记为取消状态
    }

    ImGui::PushID(id());
    ImGuiX::showDataTable(name().c_str(), dataType_, idata_, transpose_, vis_);
    ImGui::PopID();
}
