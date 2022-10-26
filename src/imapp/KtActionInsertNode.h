#pragma once
#include "KvAction.h"
#include "KgPipeline.h"


// 向node-editor直接插入新节点的辅助模板类

template<typename T>
class KtActionInsertNode : public KvAction
{
public:

	using KvAction::KvAction;

	bool trigger() override {
		auto node = std::make_shared<T>();
		KsImApp::singleton().pipeline().insertNode(node);
		state_ = KeState::k_done;
		return true;
	}

	// 每帧更新
	void update() override { }

private:
	std::shared_ptr<T> node_; // 待插入的新节点
};
