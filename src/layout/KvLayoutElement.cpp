#include "KvLayoutElement.h"
#include <assert.h>


KvLayoutElement::float_t KvLayoutElement::arrange_(const rect_t& rc, int dim)
{
	assert(rc.upper()[dim] > rc.lower()[dim]);
	assert(iRect_.lower().at(dim) == margins_.lower().at(dim));

	oRect_.lower().at(dim) = rc.lower().at(dim);
	oRect_.upper().at(dim) = rc.upper().at(dim);

	auto contentSize = iRect_.upper().at(dim) - iRect_.lower().at(dim);

	// iRect_被calcSize初始化为margins_.lower，此时只需加上偏移量即可
	iRect_.lower().at(dim) += rc.lower().at(dim);
	if (iRect_.lower().at(dim) > rc.upper().at(dim))
		iRect_.lower().at(dim) = rc.upper().at(dim);

	iRect_.upper().at(dim) = rc.upper().at(dim) - margins_.upper().at(dim);
	if (iRect_.upper().at(dim) < iRect_.lower().at(dim))
		iRect_.upper().at(dim) = iRect_.lower().at(dim);

	return contentSize;
}
