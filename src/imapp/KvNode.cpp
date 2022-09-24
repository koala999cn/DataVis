#include "KvNode.h"
#include "KuStrUtil.h"


KvNode::KvNode(const std::string_view& name, unsigned ins, unsigned outs)
	: super_(name)
{
	ins_.resize(ins);
	for (unsigned i = 0; i < ins; i++)
		ins_[i] = std::make_unique<KcPort>("in" + KuStrUtil::toString(i));

	outs_.resize(outs);
	for (unsigned i = 0; i < outs; i++)
		outs_[i] = std::make_unique<KcPort>("out" + KuStrUtil::toString(i));
}
