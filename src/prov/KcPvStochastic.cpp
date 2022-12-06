#include "KcPvStochastic.h"
#include "KuStocahsticFactory.h"


KcPvStochastic::KcPvStochastic()
	: super_("Stochastic")
{

}


unsigned KcPvStochastic::typeCount_() const
{
	return KuStocahsticFactory::typeCount();
}


const char* KcPvStochastic::typeStr_(int type) const
{
	return KuStocahsticFactory::typeName(type);
}


KvExcitor* KcPvStochastic::createExcitor_(int type)
{
	return KuStocahsticFactory::create(type);
}
