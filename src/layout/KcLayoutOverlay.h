#pragma once
#include "KvLayoutContainer.h"
#include "layout/KuLayoutUtil.h"


class KcLayoutOverlay : public KvLayoutContainer
{
	using super_ = KvLayoutContainer;

public:

	using super_::super_;

	void arrange_(int dim, float_t lower, float_t upper) override {
		super_::arrange_(dim, lower, upper);

		auto rc = innerRect();
		for (auto& i : elements()) {
			if (i == nullptr) continue;
			
			if (i->align().outter())
				rc = KuLayoutUtil::outterAlignedRect(outterRect(), i->expectRoom(), i->align());
			i->arrange_(dim, rc.lower()[dim], rc.upper()[dim]);
		}
	}


private:

	size_t calcSize_(void* cxt) const final {
		for (auto& i : elements())
			if (i) i->calcSize(cxt);
		return { 0, 0 };
	}
};