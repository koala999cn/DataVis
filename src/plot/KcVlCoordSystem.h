#pragma once
#include "KvCoordSystem.h"
#include <vlCore/Object.hpp>


class KcVlCoordSystem : public KvCoordSystem
{
public:

	KcVlCoordSystem();

	void draw(KglPaint*) const override;

private:
	void drawAxis_(KglPaint*, KgAxis*) const;
};
