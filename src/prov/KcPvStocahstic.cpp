#include "KcPvStocahstic.h"
#include "KuStocahsticFactory.h"


KcPvStocahstic::KcPvStocahstic()
	: super_("Stocahstic")
{

}


unsigned KcPvStocahstic::typeCount_() const
{
	return KuStocahsticFactory::typeCount();
}


const char* KcPvStocahstic::typeStr_(int type) const
{
	return KuStocahsticFactory::typeName(type);
}


KvExcitor* KcPvStocahstic::createExcitor_(int type)
{
	return KuStocahsticFactory::create(type);
}
