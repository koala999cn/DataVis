#include "KcLayoutFold.h"
#include "KuMatrixUtil.h"


std::vector<std::vector<KvLayoutElement*>> KcLayoutFold::layout_() const
{
	std::vector<std::vector<KvLayoutElement*>> lay;
	auto eleCount = size();
	auto wraps = maxItemsPerLine_ <= 0 ? eleCount : maxItemsPerLine_;

	int rows = (eleCount - 1) / wraps + 1;
	int cols = wraps;
	auto iter = elements().begin();
	lay.resize(rows);
	for (int i = 0; i < rows - 1; i++) {
		lay[i].resize(wraps);
		for (int j = 0; i < cols; j++, ++iter)
			lay[i][j] = (*iter);
	}

	assert(iter != elements().end());
	lay.back().reserve(wraps);
	while (iter != elements().end())
		lay.back().push_back((*iter++));

	return rowMajor_ ? lay : KuMatrixUtil::transpose(lay);
}