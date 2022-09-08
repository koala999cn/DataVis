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

	void setVisible(bool b) override;

	bool visible() const override;

	void draw(KglPaint*) const override;
};
