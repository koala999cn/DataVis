#pragma once
#include "KvLayoutContainer.h"
#include "layout/KuLayoutUtil.h"


class KcLayoutOverlay : public KvLayoutContainer
{
	using super_ = KvLayoutContainer;

public:

	using super_::super_;

	void arrange(const rect_t& rc) final {
		super_::arrange(rc);

		auto iRect = innerRect();
		for (int i = 0; i < 2; i++)
			if (rc.extent(i) == 0) iRect.setExtent(i, 0);
		for (auto& i : elements()) {
			if (i == nullptr) continue;
			
			if (i->align().outter())
				iRect = KuLayoutUtil::outterAlignedRect(rc, i->expectRoom(), i->align());
			i->arrange(iRect);
		}
	}


private:

	size_t calcSize_(void* cxt) const final {
		for (auto& i : elements())
			if (i) i->calcSize(cxt);
		return { 0, 0 };
	}
};