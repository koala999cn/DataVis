#include "KcActionDataClean.h"
#include "KcImDataView.h"
#include "KsImApp.h"
#include "KgImWindowManager.h"


KcActionDataClean::KcActionDataClean(const std::string& filepath)
    : KvAction("DataClean")
    , filepath_(filepath)
{

}


bool KcActionDataClean::trigger()
{
    KcImDataView::matrix<std::string> m;
    auto dataView = std::make_shared<KcImDataView>(filepath_, m);
    if (dataView == nullptr)
        return false;

    KsImApp::singleton().windowManager().registerInstance(dataView);
    state_ = KeState::k_triggered;
    return true;
}


void KcActionDataClean::update()
{

}
