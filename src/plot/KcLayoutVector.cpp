#include "KcLayoutVector.h"
#include "KuMatrixUtil.h"


void KcLayoutVector::setOutterRect(const rect_t& rc)
{
	;
}


KcLayoutVector::size_t KcLayoutVector::calcContentSize_() const
{
	auto lay = layout_();

	size_t contentSize(0);

	bool autoWidth(false), autoHeight(false);
	for (auto& r : lay) {
		double rowHeight(0), rowWidth(0);
		
		for (auto& c : r) {
			if (c) {
				c->calcContentSize();
				auto sz = c->expectRoom();
				rowWidth += sz.x();
				rowHeight = std::max(rowHeight, sz.y());

				if (c->contentSize().x() == auto_fit_size)
					autoWidth = true;
				if (c->contentSize().y() == auto_fit_size)
					autoHeight = true;
			}
		}

		contentSize.x() = std::max(rowWidth, contentSize.x());
		contentSize.y() += rowHeight;
	}

	// 只要有一个元素是autosize，则设定size为autosize
	if (autoWidth)
		contentSize.x() = auto_fit_size;
	else
		contentSize.x() += colSpacing_ * (lay[0].size() - 1);

	if (autoHeight) 
		contentSize.y() = auto_fit_size;
	else 
		contentSize.y() += rowSpacing_ * (lay.size() - 1);

	return contentSize;
}


std::vector<std::vector<KvLayoutElement*>> KcLayoutVector::layout_() const
{
	std::vector<std::vector<KvLayoutElement*>> lay;
	auto eleCount = elements_.size();
	auto wraps = wraps_ <= 0 ? eleCount : wraps_;

	int rows = (eleCount - 1) / wraps + 1;
	int cols = wraps;
	auto iter = elements_.begin();
	lay.resize(rows);
	for (int i = 0; i < rows - 1; i++) {
		lay[i].resize(wraps);
		for (int j = 0; i < cols; j++, ++iter)
			lay[i][j] = iter->get();
	}

	assert(iter != elements_.end());
	lay.back().reserve(wraps);
	while (iter != elements_.end())
		lay.back().push_back(iter++->get());

	return rowMajor_ ? lay : KuMatrixUtil::transpose(lay);
}