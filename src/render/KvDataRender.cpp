#include "KvDataRender.h"
#include "prov/KvDataProvider.h"
#include <assert.h>


bool KvDataRender::onNewLink(KcPortNode* from, KcPortNode* to)
{
	assert(to->parent().lock().get() == this);
	assert(to->index() < inPorts());

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(from->parent().lock());
	return prov && permitInput(prov->spec(from->index()), to->index());
}
