#include "KuLayoutUtil.h"
#include "KeAlignment.h"
#include <assert.h>


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


KuLayoutUtil::rect_t KuLayoutUtil::outterAlignedRect(const rect_t& base, const size_t& contentSize, int align)
{
    assert(KeAlignment(align).outter());

    auto sz = contentSize;
    for (int i = 0; i < 2; i++)
        if (sz[i] == 0 || base.extent(i) == 0)
            sz[i] = base.extent(i);
    
    // 初始化lower点为center对齐状态下的数值
    point_t lower(base.center().x() - sz.x() / 2, base.center().y() - sz.y() / 2);

    if (align & KeAlignment::k_horz_first) { // 布局于base的左/右侧
 
        if (align & KeAlignment::k_left)
            lower.x() = base.lower().x() - sz.x();
        else if (align & KeAlignment::k_right)
            lower.x() = base.upper().x();
        else {
            assert(false);
        }

        if (align & KeAlignment::k_bottom)
            lower.y() = base.upper().y() - sz.y();
        else if (align & KeAlignment::k_vcenter)
            ; // 缺省已居中
        else { // align & KeAlignment::k_top 或fill
            lower.y() = base.lower().y();
            if (!(align & KeAlignment::k_top))
                sz.y() = base.extent(1); // yfill
        }
    }
    else { // 布局于base的上/下侧

        if (align & KeAlignment::k_top)
            lower.y() = base.lower().y() - sz.y();
        else if (align & KeAlignment::k_bottom)
            lower.y() = base.upper().y();
        else {
            assert(false);
        }

        if (align & KeAlignment::k_right)
            lower.x() = base.upper().x() - sz.x();
        else if (align & KeAlignment::k_hcenter)
            ; // 缺省已居中
        else { // align & KeAlignment::k_left 或fill
            lower.x() = base.lower().x();
            if (!(align & KeAlignment::k_left))
                sz.x() = base.extent(0); // xfill
        }
    }

    return { lower, lower + sz };
}


KuLayoutUtil::rect_t KuLayoutUtil::innerAlignedRect(const rect_t& base, const size_t& contentSize, int align, bool fillAsCenter)
{
    assert(KeAlignment(align).inner());

    auto sz = contentSize;
    for (int i = 0; i < 2; i++)
        if (sz[i] == 0 || base.extent(i) == 0)
            sz[i] = base.extent(i);

    point_t lower(base.lower());

    if (align & KeAlignment::k_right)
        lower.x() = base.upper().x() - sz.x();
    else if ((align & KeAlignment::k_hcenter) || (fillAsCenter && KeAlignment(align).hfill()))
        lower.x() = base.center().x() - sz.x() / 2;

    if (align & KeAlignment::k_bottom)
        lower.y() = base.upper().y() - sz.y();
    else if ((align & KeAlignment::k_vcenter) || (fillAsCenter && KeAlignment(align).vfill()))
        lower.y() = base.center().y() - sz.y() / 2;

    return { lower, lower + sz };
}
