#pragma once
#include "KvAction.h"
#include "KgPipeline.h"


// ��node-editorֱ�Ӳ����½ڵ�ĸ���ģ����

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

	// ÿ֡����
	void update() override { }

private:
	std::shared_ptr<T> node_; // ��������½ڵ�
};
