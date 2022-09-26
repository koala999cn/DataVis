#include "KcImDataView.h"
#include "KuPathUtil.h"


KcImDataView::KcImDataView(const std::string& source, const matrix<std::string>& rawData)
    : KvImWindow(KuPathUtil::fileName(source))
    , source_(source)
    , rawData_(rawData)
{

}


void KcImDataView::update()
{

}
