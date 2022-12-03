#pragma once
#include "KvLayoutContainer.h"


class KcLayoutOverlay : public KvLayoutContainer
{
	using super_ = KvLayoutContainer;

public:

	using super_::super_;

	void arrange(const rect_t& rc) final {
		super_::arrange(rc);

		auto iRect = innerRect();
		if (rc.width() == 0) iRect.upper()[0] = iRect.lower()[0];
		if (rc.height() == 0) iRect.upper()[1] = iRect.lower()[1];
		for (auto& i : elements())
			if (i) i->arrange(iRect);
	}


private:

	size_t calcSize_(void* cxt) const final {
		for (auto& i : elements())
			if (i) i->calcSize(cxt);
		return { 0, 0 };
	}
};