#include "KuLayoutUtil.h"
#include "KeAlignment.h"


KuLayoutUtil::rect_t KuLayoutUtil::anchorAlignedRect(const point_t& anchor, const size_t& box, int align)
{
	auto lower = anchor;

	if (align & KeAlignment::k_bottom)
		lower.y() -= box.y();
	else if (align & KeAlignment::k_top)
		lower.y();
	else // k_vcenter
		lower.y() -= box.y() * 0.5;

	if (align & KeAlignment::k_left)
		lower.x();
	else if (align & KeAlignment::k_right)
		lower.x() -= box.x();
	else // k_hcenter
		lower.x() -= box.x() * 0.5;

	return { lower, lower + box };
}
