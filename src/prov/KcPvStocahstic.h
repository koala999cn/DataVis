#pragma once
#include "KvPvExcitor.h"


class KcPvStocahstic : public KvPvExcitor
{
	using super_ = KvPvExcitor;

public:

	KcPvStocahstic();


private:

	unsigned typeCount_() const final;
	const char* typeStr_(int type) const final;
	KvExcitor* createExcitor_(int type) final;

};