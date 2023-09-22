#include "KvDecorator.h"
#include "KvPaint.h"


void KvDecorator::draw(KvPaint* paint) const
{
    auto box = boundingBox();

    if (showBkgnd_ && bkgnd_.visible()) {
        paint->apply(bkgnd_);
        paint->fillRect(box.lower(), box.upper());
    }

    if (showBorder_ && border_.visible()) {
        paint->apply(border_);
        paint->drawRect(box.lower(), box.upper());
    }
}
