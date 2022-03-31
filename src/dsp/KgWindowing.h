#pragma once
#include "KgMultiply.h"
#include "KuWindowFactory.h"


class KgWindowing : public KgMultiply
{
public:
	KgWindowing(int type, kIndex nx, ...);
};

