#include "KcImDataView.h"
#include "KuPathUtil.h"


KcImDataView::KcImDataView(const std::string& source, const matrixd& idata, std::shared_ptr<KvData>& odata)
    : KvImModalWindow(KuPathUtil::fileName(source))
    , idata_(idata), odata_(odata)
{

}


void KcImDataView::updateImpl_()
{

}