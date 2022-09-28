#include "KcActionInsertDataNode.h"
#include "KcImDataView.h"
#include <assert.h>


KcActionInsertDataNode::KcActionInsertDataNode(const std::string& filepath, const matrixd& idata)
    : KvAction("InsertDataNode")
    , filepath_(filepath)
    , idata_(idata)
{

}


bool KcActionInsertDataNode::trigger()
{
    // 创建数据窗口
    dataView_ = std::make_unique<KcImDataView>(filepath_, idata_, odata_);
    if (dataView_ == nullptr) {
        state_ = KeState::k_failed;
        return false;
    }

    if(dataView_->visible())
        dataView_->open();

    state_ = KeState::k_triggered;
    return true;
}


void KcActionInsertDataNode::update()
{
    assert(dataView_ != nullptr);

    if (dataView_->opened())
        dataView_->update();
    else {
        assert(!dataView_->opened());
        state_ = odata_ == nullptr ? KeState::k_cancelled : KeState::k_done;
        dataView_ = nullptr;
    }
}
