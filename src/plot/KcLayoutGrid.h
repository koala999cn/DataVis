#pragma once
#include "KcLayoutVector.h"


class KcLayoutGrid : public KcLayoutVector
{
public:


private:
	std::vector<std::unique_ptr<KcLayoutVector>> rows_;
};
