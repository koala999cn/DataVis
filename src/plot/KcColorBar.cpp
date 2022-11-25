#include "KcColorBar.h"
#include "KvPlottable.h"
#include "KvPaint.h"
#include "KuAlignment.h"


KcColorBar::KcColorBar(KvPlottable* plt)
    : super_("ColorBar")
    , plt_(plt)
    , align_(k_align_right)
{
    assert(plt_ != nullptr);
}


void KcColorBar::draw(KvPaint* paint) const
{

}


KcColorBar::aabb_t KcColorBar::boundingBox() const
{
    assert(false);
    return {};
}


KcColorBar::point2i KcColorBar::calcSize(KvPaint* paint) const
{
    if (align_ & k_align_left || align_ & k_align_right)
        return { barWidth_, barLength_ };
    else 
        return { barLength_, barWidth_ };
}
