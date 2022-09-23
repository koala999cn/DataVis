#include "KvNode.h"


KvNode::KvNode(const std::string_view& name, unsigned ins, unsigned outs)
	: name_(name)
{
	static int idseed(0);
	id_ = idseed++;

	ins_.resize(ins);
	for(unsigned i = 0; i < ins; i++)
		std::get<0>(ins_[i]) = idseed++;

	outs_.resize(outs);
	for (unsigned i = 0; i < outs; i++)
		std::get<0>(outs_[i]) = idseed++;
}
