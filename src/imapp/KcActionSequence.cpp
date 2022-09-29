#include "KcActionSequence.h"
#include <assert.h>


KcActionSequence::KcActionSequence(const std::string_view& name, 
	const std::string_view& desc)
	: KvAction(name), desc_(desc)
{
	if (desc_.empty())
		desc_ = name;
}


auto KcActionSequence::desc() const -> const std::string_view
{
	return desc_;
}


bool KcActionSequence::trigger()
{
	if (seqs_.empty())
		return false;

	// TODO: �����и�action��״̬�Ƿ���Ҫ��reset

	bool res = seqs_.front()->trigger();
	state_ = seqs_.front()->state();
	return res;
}


void KcActionSequence::update()
{
	assert(triggered());

	auto iter = seqs_.begin();
	for (; iter != seqs_.end(); iter++) { // seqs_�б���һ��Ԫ�ش��ڴ���̬
		if ((*iter)->triggered()) {
			(*iter)->update();
			
			if ((*iter)->done()) { // ��ǰaction������������һ��״̬
				auto st = (*iter++)->state();
				if (iter == seqs_.end()) {
					state_ = st;
				}
				else {
					(*iter)->trigger();
					state_ = (*iter)->state();
				}

				break;
			}

			state_ = (*iter)->state();
		}
	}
}


void KcActionSequence::addAction(std::shared_ptr<KvAction> act)
{
	seqs_.push_back(act);
}