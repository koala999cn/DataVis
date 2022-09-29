#include "KcActionInsertDataNode.h"
#include "KcImDataView.h"
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
        if (odata_) {
            state_ = KeState::k_done;
            auto node = std::make_shared<KcPvData>(KuPathUtil::fileName(filepath_), odata_);
            KsImApp::singleton().windowManager().getStatic<KcImNodeEditor>()->insertNode(node);
        }
        else {
            state_ = KeState::k_cancelled;
        }
        dataView_ = nullptr;
    }
}
