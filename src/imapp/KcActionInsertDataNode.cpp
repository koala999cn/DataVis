#include "KcActionInsertDataNode.h"
#include "KcImDataMaker.h"
#include <assert.h>
#include "KsImApp.h"
#include "KgPipeline.h"
#include "prov/KcPvData.h"
#include "KuPathUtil.h"


KcActionInsertDataNode::KcActionInsertDataNode(const std::string& filepath, const matrixd& idata)
    : KvAction("InsertDataNode")
    , filepath_(filepath)
    , idata_(idata)
{

}


bool KcActionInsertDataNode::trigger()
{
    // 创建数据窗口
    dataMaker_ = std::make_unique<KcImDataMaker>(filepath_, idata_, odata_);
    if (dataMaker_ == nullptr) {
        state_ = KeState::k_failed;
        return false;
    }

    state_ = KeState::k_triggered;
    return true;
}


namespace kPrivate
{
    std::string localToUtf8(const std::string& str);
}

void KcActionInsertDataNode::update()
{
    assert(dataMaker_ != nullptr);

    if (dataMaker_->visible())
        dataMaker_->update();
    else {
        if (odata_) {
            state_ = KeState::k_done;
            auto node = std::make_shared<KcPvData>(
                kPrivate::localToUtf8(KuPathUtil::fileName(filepath_)), odata_);
            KsImApp::singleton().pipeline().insertNode(node);
        }
        else {
            state_ = KeState::k_cancelled;
        }
        dataMaker_ = nullptr;
    }
}
