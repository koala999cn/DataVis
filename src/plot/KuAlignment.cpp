#if 0

#include "KuAlignment.h"


point2d KuAlignment::position(int align, const point2d& sz, const rect& rc)
{
    point2d pos(rc.center().x() - sz.x() / 2, rc.center().y() - sz.y() / 2);

    if (align & k_align_left)
        pos.x() = rc.lower().x();
    else if (align & k_align_right)
        pos.x() = rc.upper().x() - sz.x();

    if (align & k_align_top)
        pos.y() = rc.lower().y();
    else if (align & k_align_bottom)
        pos.y() = rc.upper().y() - sz.y();

    pos = pos.ceil(pos, rc.lower());

    // 若位于外部，则进一步调整pos
    if (align & k_align_horz_first) { // 外侧，左右优先
        if (align & k_align_left)
            pos.x() -= sz.x();
        else if (align & k_align_right)
            pos.x() += sz.x();
        else if (align & k_align_top)
            pos.y() -= sz.y();
        else if (align & k_align_bottom)
            pos.y() += sz.y();
    }
    else if (align & k_align_vert_first) { // 外侧，上下优先
        if (align & k_align_top)
            pos.y() -= sz.y();
        else if (align & k_align_bottom)
            pos.y() += sz.y();
        else if (align & k_align_left)
            pos.x() -= sz.x();
        else if (align & k_align_right)
            pos.x() += sz.x();
    }

    return pos;
}


KuAlignment::rect KuAlignment::layout(int align, const point2d& szBox, const rect& rcAll)
{
	if (inside(align))
		return rcAll;

	auto rc(rcAll);

	if (align & k_align_horz_first) {
		if (align & k_align_left)
			rc.lower().x() += szBox.x();
		else if (align & k_align_right)
			rc.upper().x() -= szBox.x();
		else if (align & k_align_top)
			rc.lower().y() += szBox.y();
		else if (align & k_align_bottom)
			rc.upper().y() -= szBox.y();
	}
	else if (align & k_align_vert_first) {
		if (align & k_align_top)
			rc.lower().y() += szBox.y();
		else if (align & k_align_bottom)
			rc.upper().y() -= szBox.y();
		else if (align & k_align_left)
			rc.lower().x() += szBox.x();
		else if (align & k_align_right)
			rc.upper().x() -= szBox.x();
	}

	return rc;
}
#endif