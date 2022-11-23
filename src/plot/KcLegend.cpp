#include "KcLegend.h"
#include "KvPlottable.h"
#include "KvPaint.h"


KcLegend::KcLegend()
    : super_("Legend")
{

}


point2f KcLegend::calcSize(KvPaint* paint) const
{
    point2f legSize = innerMargin_ * 2;
    legSize += outterMargin_ * 2;

    if (items_.empty())
        return legSize;

    auto labelSize = maxLabelSize_(paint);

    auto lay = layouts_();
    assert(lay.x() > 0 && lay.y() > 0);
    auto itemWidth = iconSize_.x() + iconTextPadding_ + labelSize.x();
    auto itemHeight = std::max(labelSize.y(), iconSize_.y());
    legSize.x() += itemWidth + (lay.y() - 1) * (itemWidth + itemSpacing_.x());
    legSize.y() += itemHeight + (lay.x() - 1) * (itemHeight + itemSpacing_.y());

    return legSize;
}


KtPoint<int, 2> KcLegend::maxLabelSize_(KvPaint* paint) const
{
    KtPoint<int, 2> maxSize(0, 0);

    for (unsigned i = 0; i < items_.size(); ++i) {
        auto& label = items_[i]->name();
        auto labelSize = paint->textSize(label.c_str());
        maxSize = maxSize.ceil(maxSize, labelSize);
    }

    return maxSize;
}


KtPoint<int, 2> KcLegend::layouts_() const
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
    // 在legned的局部空间执行绘制操作，确保paint已被正确设置
    using point3 = KvPaint::point3;

    auto sz = calcSize(paint);
    sz -= outterMargin_;
    paint->apply(border_);
    paint->drawRect(point3(outterMargin_.x(), outterMargin_.y(), 0), point3(sz.x(), sz.y(), 0));

    auto itemSize = maxLabelSize_(paint);
    itemSize.x() += iconSize_.x() + iconTextPadding_;
    itemSize.y() = std::max(itemSize.y(), iconSize_.y());

    auto lay = layouts_();
    assert(lay.x() > 0 && lay.y() > 0);

    auto itemPos = outterMargin_ + innerMargin_;
    unsigned rowStride = lay.y(), colStride = lay.x();

    KvPaint::rect itemRect(itemPos, itemPos + itemSize);
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


void KcLegend::drawItem_(KvPaint* paint, KvPlottable* item, const rect& rc) const
{
    using point3 = KvPaint::point3;

    //paint->drawRect(point3(rc.lower().x(), rc.lower().y(), 0), 
    //    point3(rc.upper().x(), rc.upper().y(), 0)); 

    auto iconPos = rc.lower();
    iconPos.y() += (rc.height() - iconSize_.y()) / 2;
    paint->setColor(item->majorColor(0));
    paint->fillRect(point3(iconPos.x(), iconPos.y(), 0), 
        point3(iconPos.x() + iconSize_.x(), iconPos.y() + iconSize_.y(), 0));

    auto lablePos = rc.lower();
    lablePos.x() += iconSize_.x() + iconTextPadding_;
    lablePos.y() += rc.height() / 2;
    paint->setColor(clrText_);
    paint->apply(fontText_);
    paint->drawText(point3(lablePos.x(), lablePos.y(), 0), item->name().c_str(), k_align_left);
}


KcLegend::aabb_type KcLegend::boundingBox() const
{
    assert(false); // TODO:
    return {};
}
