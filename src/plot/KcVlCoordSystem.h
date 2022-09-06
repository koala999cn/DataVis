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

	void update(KvPlot*) const override;

private:
	vl::ref<vl::ActorTree> actorTree_;
};