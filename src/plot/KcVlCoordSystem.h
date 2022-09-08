#pragma once
#include "KvCoordSystem.h"
#include <vlCore/Object.hpp>

namespace vl {
	class ActorTree;
}

class KcVlCoordSystem : public KvCoordSystem
{
public:

	KcVlCoordSystem();

	void draw(KglPaint*) const override;
};
