#pragma once
#include "KtUniObject.h"


class KvAction : public KtUniObject<KvAction>
{
public:
	using super_ = KtUniObject<KvAction>;

	// action��״̬����
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


	// ���ڸ�action��������Ϣ����Ҫ������ʾtooltip
	virtual auto desc() const -> const std::string_view = 0;

	// ����action������true��ʾ�ɹ�������false��ʾ����ʧ��
	virtual bool trigger() = 0;

	// ÿ֡����
	virtual void update() = 0;

	// TODO: ֧��undo/redo����
	// virtual void undo() = 0;
	// virtual void redo() = 0;


	// ����״̬�йغ���

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
	KeState state_; // action�ĵ�ǰ״̬���������ฺ�����
};
