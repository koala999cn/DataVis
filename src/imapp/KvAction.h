#pragma once
#include "KtUniObject.h"


class KvAction : public KtUniObject<KvAction>
{
public:
	using super_ = KtUniObject<KvAction>;

	// action的状态定义
	enum class KeState
	{
		k_ready,
		k_triggered,
		k_done,
		k_cancelled,
		k_disabled
	};


	explicit KvAction(const std::string_view& name) 
		: super_(name), state_(KeState::k_ready) {}


	// 关于该action的描述信息，主要用于显示tooltip
	virtual auto desc() const -> const std::string_view = 0;

	// 触发action，返回true表示成功触发，false表示触发失败
	virtual bool trigger() = 0;

	// 每帧更新
	virtual void update() = 0;

	// TODO: 支持undo/redo操作
	// virtual void undo() = 0;
	// virtual void redo() = 0;


	// 几个状态有关函数

	KeState state() const { return state_; }

	bool ready() const { 
		return state() == KeState::k_ready; 
	}

	bool triggered() const {
		return state() == KeState::k_triggered;
	}

	bool done() const {
		return state() == KeState::k_done;
	}

	bool cancelled() const {
		return state() == KeState::k_cancelled;
	}

	bool disabled() const {
		return state() == KeState::k_disabled;
	}

protected:
	KeState state_; // action的当前状态，由派生类负责更新
};
