#include "KcActionInsertDataNode.h"
#include "KcImDataMaker.h"
#include <assert.h>
#include "KsImApp.h"
#include "KgImWindowManager.h"
#include "KcImNodeEditor.h"
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

    if(dataMaker_->visible())
        dataMaker_->open();

    state_ = KeState::k_triggered;
    return true;
}


void KcActionInsertDataNode::update()
{
    assert(dataMaker_ != nullptr);

    if (dataMaker_->opened())
        dataMaker_->update();
    else {
        assert(!dataMaker_->opened());
        if (odata_) {
            state_ = KeState::k_done;
            auto node = std::make_shared<KcPvData>(KuPathUtil::fileName(filepath_), odata_);
            KsImApp::singleton().windowManager().getStatic<KcImNodeEditor>()->insertNode(node);
        }
        else {
            state_ = KeState::k_cancelled;
        }
        dataMaker_ = nullptr;
    }
}
