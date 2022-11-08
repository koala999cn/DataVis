#include "KcImDataView.h"
#include "prov/KvDataProvider.h"
#include "imguix.h"


KcImDataView::KcImDataView(std::shared_ptr<KvDataProvider> prov)
    : KvImWindow("DataView - " + prov->name())
    , prov_(prov)
{
    dynamic_ = true;
    deleteOnClose_ = true;
}


KcImDataView::KcImDataView(std::shared_ptr<KvData> data, std::string_view sv)
    : KvImWindow("DataView - " + std::string(sv))
    , data_(data)
{
    dynamic_ = true;
    deleteOnClose_ = true;
}


void KcImDataView::updateImpl_()
{
    auto prov = prov_.lock();

    if (prov) {
        auto data = prov->fetchData(0);
        if (data)
            ImGuiX::showDataTable(*data);
    }
    else if (data_)
        ImGuiX::showDataTable(*data_);
}
