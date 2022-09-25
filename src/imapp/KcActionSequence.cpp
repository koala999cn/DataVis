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

	bool res = seqs_.front()->trigger();
	state_ = seqs_.front()->state();
	return res;
}


void KcActionSequence::update()
{
	assert(triggered());
	for (auto& i : seqs_) { // seqs_中必有一个元素处于触发态
		if (i->triggered()) {
			i->update();
			break;
		}
	}
}


void KcActionSequence::addAction(std::shared_ptr<KvAction> act)
{
	seqs_.push_back(act);
}