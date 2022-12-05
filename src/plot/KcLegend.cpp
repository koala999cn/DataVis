#include "KcLegend.h"
#include "KvPlottable.h"
#include "KvPaint.h"


KcLegend::KcLegend()
    : super_("Legend")
{
    align() = location_ = KeAlignment::k_right | KeAlignment::k_top | KeAlignment::k_horz_first | KeAlignment::k_outter;
    setMargins({ point_t(7, 5), point_t(7, 5) });
}


KcLegend::size_t KcLegend::calcSize_(void* cxt) const
{
    point2f legSize = innerMargin_ * 2;

    if (items_.empty())
        return legSize;

    auto paint = (KvPaint*)cxt;
    auto labelSize = maxLabelSize_(paint);

    auto lay = layouts_();
    assert(lay.x() > 0 && lay.y() > 0);
    auto itemWidth = iconSize_.x() + iconTextPadding_ + labelSize.x();
    auto itemHeight = std::max(labelSize.y(), iconSize_.y());
    legSize.x() += itemWidth + (lay.y() - 1) * (itemWidth + itemSpacing_.x());
    legSize.y() += itemHeight + (lay.x() - 1) * (itemHeight + itemSpacing_.y());

    return legSize;
}


KcLegend::point2i KcLegend::maxLabelSize_(KvPaint* paint) const
{
    point2i maxSize(0, 0);

    for (unsigned i = 0; i < items_.size(); ++i) {
        auto& label = items_[i]->name();
        auto labelSize = paint->textSize(label.c_str());
        maxSize = maxSize.ceil(maxSize, labelSize);
    }

    return maxSize;
}


KcLegend::point2i KcLegend::layouts_() const
{
    auto itemCount = items_.size();

    int rows = 1;
    int cols = itemCount;
    if (maxItemsPerRow_ > 0) {
        rows = (itemCount - 1) / maxItemsPerRow_ + 1;
        cols = maxItemsPerRow_;
    }

    assert(rows * cols >= itemCount);
    if (!rowMajor_) std::swap(rows, cols);

    return { rows, cols };
}


void KcLegend::draw(KvPaint* paint) const
{
    using point3 = KvPaint::point3;

    // 配置paint，以便在legned的局部空间执行绘制操作
    paint->pushCoord(KvPaint::k_coord_screen);
    // TODO; 设置clipRect
    
    paint->apply(border_);
    paint->drawRect(iRect_);

    if (!items_.empty()) {

        auto itemSize = maxLabelSize_(paint);
        itemSize.x() += iconSize_.x() + iconTextPadding_;
        itemSize.y() = std::max(itemSize.y(), iconSize_.y());

        auto lay = layouts_();
        assert(lay.x() > 0 && lay.y() > 0);

        auto itemPos = iRect_.lower() + innerMargin_;
        unsigned rowStride = lay.y(), colStride = lay.x();

        rect_t itemRect(itemPos, itemPos + itemSize);
        unsigned rowIdx = 0;
        for (unsigned r = 0; r < lay.x(); r++, rowIdx += rowStride) {
            auto itemIdx = rowIdx;
            auto rcNow = itemRect;
            for (unsigned c = 0; c < lay.y(); c++, itemIdx += colStride) {
                drawItem_(paint, items_[itemIdx], rcNow);

                rcNow.lower().x() += itemSize.x() + itemSpacing_.x();
                rcNow.upper().x() += itemSize.x() + itemSpacing_.x();
            }

            itemRect.lower().y() += itemSize.y() + itemSpacing_.y();
            itemRect.upper().y() += itemSize.y() + itemSpacing_.y();
        }

    }

    paint->popCoord();
}


void KcLegend::drawItem_(KvPaint* paint, KvPlottable* item, const rect_t& rc) const
{
    using point3 = KvPaint::point3;

    auto iconPos = rc.lower();
    iconPos.y() += (rc.height() - iconSize_.y()) / 2;
    paint->setColor(item->majorColor(0));
    paint->fillRect(iconPos, iconPos + iconSize_);

    auto lablePos = rc.lower();
    lablePos.x() += iconSize_.x() + iconTextPadding_;
    lablePos.y() += rc.height() / 2;
    paint->setColor(clrText_);
    paint->apply(fontText_);
    paint->drawText(lablePos, item->name().c_str(), KeAlignment::k_left);
}


KcLegend::aabb_t KcLegend::boundingBox() const
{
    assert(false); // TODO:
    return {};
}
