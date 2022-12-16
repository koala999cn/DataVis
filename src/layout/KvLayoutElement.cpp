#include "KvLayoutElement.h"
#include <assert.h>


void KvLayoutElement::arrange_(const rect_t& rc, int dim)
{
	// assert(rc.upper()[dim] > rc.lower()[dim]);

	oRect_.lower()[dim] = rc.lower()[dim];
	oRect_.upper()[dim] = rc.upper()[dim];
	iRect_.lower()[dim] = rc.lower()[dim] + margins_.lower()[dim];
	iRect_.upper()[dim] = rc.upper()[dim] - margins_.upper()[dim];

	if (iRect_.upper().at(dim) < iRect_.lower().at(dim))
		iRect_.setExtent(dim, 0);
	else if (contentSize_[dim] > 0 && iRect_.extent(dim) > contentSize_[dim]) {
		// 根据align对iRect进行调整
		const int left[] = { align_ & KeAlignment::k_left, align_ & KeAlignment::k_top };
		const int mid[] = { align_ & KeAlignment::k_hcenter, align_ & KeAlignment::k_vcenter };
		const int right[] = { align_ & KeAlignment::k_right, align_ & KeAlignment::k_bottom };
		
		if (left[dim]) {
			iRect_.setExtent(dim, contentSize_[dim]);
		}
		else if (right[dim]) {
			iRect_.lower()[dim] = iRect_.upper()[dim] - contentSize_[dim];
		}
		else if (mid[dim]) {
			auto delta = iRect_.upper().at(dim) - iRect_.lower().at(dim) - contentSize_[dim];
			iRect_.lower()[dim] += delta * 0.5;
			iRect_.upper()[dim] -= delta * 0.5;
		}
		else {
			; // 拉伸contentBox到iRect
		}
	}
}
