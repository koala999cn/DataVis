#include "KcLayoutGrid.h"


KcLayoutGrid::size_t KcLayoutGrid::calcContentSize_() const
{
	size_t contentSize(0);

	for (auto& r : elements_) {
		double rowHeight(0), rowWidth(0);
		for (auto& c : r) {
			auto sz = c->calcContentSize();
			rowWidth += sz.x();
			rowHeight = std::max(rowHeight, sz.y());
		}

		contentSize.x() = std::max(rowWidth, contentSize.x());
		contentSize.y() += rowHeight;
	}

	return contentSize;
}


void KcLayoutGrid::setOutterRect(const rect_t& rc)
{
	if (rc.height() > 0) {

	}

	if (rc.width() > 0) {

	}
}
