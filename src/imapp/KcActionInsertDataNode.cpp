#include "KcActionInsertDataNode.h"
#include "KcImDataView.h"
#include <assert.h>


KcActionInsertDataNode::KcActionInsertDataNode(const std::string& filepath, const matrixd& idata)
    : KvAction("InsertDataNode")
{

}


bool KcActionInsertDataNode::trigger()
{
    // 创建数据窗口
    dataView_ = std::make_unique<KcImDataView>();
    if (dataView_ == nullptr) {
        state_ = KeState::k_failed;
        return false;
    }

    state_ = dataView_->visible() ? KeState::k_triggered : KeState::k_done;
    return true;
}


void KcActionInsertDataNode::update()
{
    assert(dataView_ != nullptr);

    if (dataView_->visible())
        dataView_->update();
    else
        state_ = odata_ == nullptr ? KeState::k_cancelled : KeState::k_done;
}
