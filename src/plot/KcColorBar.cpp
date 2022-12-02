#include "KcColorBar.h"
#include "KvPlottable.h"
#include "KvPaint.h"


KcColorBar::KcColorBar(KvPlottable* plt)
    : KvRenderable("ColorBar")
    , plt_(plt)
    , location_(KeAlignment::k_right)
{
    assert(plt_ != nullptr);
    margins_ = margins_t({ 5, 5 }, { 5, 5 });
}


#include "KtGradient.h"
void KcColorBar::draw(KvPaint* paint) const
{
    paint->pushCoord(KvPaint::k_coord_screen);

    auto box = innerRect();

    bool showBorder = true;

    if (showBorder) {
        paint->setColor({ 0, 0, 0, 1 });
        paint->drawRect({ box.lower().x(), box.lower().y(), 0 },
            { box.upper().x(), box.upper().y(), 0 });
    }

    KtGradient<double, color4f> grad;
    grad.setAt(0, { 1, 0, 0, 1 });
    grad.setAt(0.5, { 1, 1, 0, 1 });
    grad.setAt(1, { 0, 0, 1, 1 });
    int offset = showBorder ? 1 : 0;
    if (location() & KeAlignment::k_horz_first) {
        for (int i = offset; i < box.height(); i++) {
            double a = (double)i / (box.height() - 1);
            auto& clr = grad.getAt(a);
            paint->setColor(clr);
            paint->drawLine({ box.lower().x() + offset, box.lower().y() + i, 0 },
                { box.upper().x(), box.lower().y() + i, 0 });
        }
    }
    else {

    }

    paint->popCoord();
}


KcColorBar::aabb_t KcColorBar::boundingBox() const
{
    assert(false);
    return {};
}


KcColorBar::size_t KcColorBar::calcSize_(void*) const
{
    if (location() & KeAlignment::k_horz_first)
        return { barWidth_, barLength_ };
    else 
        return { barLength_, barWidth_ };
}
