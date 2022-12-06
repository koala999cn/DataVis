#pragma once
#include "KvPvExcitor.h"


class KcPvStochastic : public KvPvExcitor
{
	using super_ = KvPvExcitor;

public:

	KcPvStochastic();


private:

	unsigned typeCount_() const final;
	const char* typeStr_(int type) const final;
	KvExcitor* createExcitor_(int type) final;

};