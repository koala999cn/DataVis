#include "KvDataRender.h"
#include "prov/KvDataProvider.h"
#include "imapp/KsImApp.h"
#include "imapp/KgPipeline.h"
#include <assert.h>


bool KvDataRender::onNewLink(KcPortNode* from, KcPortNode* to)
{
	assert(to->parent().lock().get() == this);
	assert(to->index() < inPorts());

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(from->parent().lock());
	return prov && !working_() // 运行时不接受新的链接
		&& permitInput(prov->spec(from->index()), to->index());
}


bool KvDataRender::onInputChanged(KcPortNode* outPort, unsigned inPort)
{
	assert(!working_());

	auto prov = std::dynamic_pointer_cast<KvDataProvider>(outPort->parent().lock());
	if (!permitInput(prov->spec(outPort->index()), inPort))
		return false;

	output();

	return true;
}


bool KvDataRender::working_() const
{
	return KsImApp::singleton().pipeline().running();
}
