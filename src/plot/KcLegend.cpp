#include "KcLegend.h"
#include "KvPlottable.h"
#include "KvPaint.h"
#include "KuStrUtil.h"


KcLegend::KcLegend()
    : super_("Legend")
{
    align() = location_ = KeAlignment::k_left | KeAlignment::k_top | KeAlignment::k_horz_first;
    
    innerMargins_.lower() = point_t(7, 5);
    innerMargins_.upper() = point_t(7, 4);
    margins() = innerMargins_;
}


KcLegend::size_t KcLegend::calcSize_(void* cxt) const
{
    point2f legSize = innerMargins_.lower() + innerMargins_.upper();

    if (plts_.empty())
        return legSize;

    for (auto i : plts_)
        i->updateColorMappingPalette(); // 此处强制同步

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


point2f KcLegend::maxLabelSize_(KvPaint* paint) const
{
    point2f maxSize(0, 0);

    for (unsigned i = 0; i < plts_.size(); ++i) {
        for (unsigned ch = 0; ch < plts_[i]->majorColorsNeeded(); ch++) {
            auto label = itemLabel_(plts_[i], ch);
            auto labelSize = paint->textSize(label.c_str());
            maxSize = maxSize.ceil(maxSize, labelSize);
        }
    }

    return maxSize;
}


KcLegend::point2i KcLegend::layouts_() const
{
    auto items = itemCount();

    int rows = 1;
    int cols = items;
    if (maxItemsPerRow_ > 0) {
        rows = (items - 1) / maxItemsPerRow_ + 1;
        cols = maxItemsPerRow_;
    }

    assert(rows * cols >= items);
    if (!rowMajor_) std::swap(rows, cols);

    return { rows, cols };
}


void KcLegend::draw(KvPaint* paint) const
{
    assert(visible());

    using point3 = KvPaint::point3;

    auto rc = paint->viewport();
    rc = rc.intersection(iRect_);
    if (rc.isNull()) return;

    // NB: ogl实现对clipRect进行了修正，此处可不加1，
    // 但由于目前使用ImGui绘制边框，所以此处加1，否则右边框有时会被裁剪掉
    rc.upper().x() += 1; 

    paint->pushClipRect(rc);

    // 配置paint，以便在legned的局部空间执行绘制操作
    paint->pushCoord(KvPaint::k_coord_screen);
    
    if (showBkgnd_ && bkgnd_.visible()) {
        paint->apply(bkgnd_);
        paint->fillRect(iRect_);
    }

    if (showBorder_ && border_.visible()) {
        paint->apply(border_);
        paint->drawRect(iRect_);
    }

    if (!plts_.empty()) 
        drawItems_(paint);
    
    paint->popCoord();
    paint->popClipRect();
}


void KcLegend::drawItem_(KvPaint* paint, KvPlottable* plt, unsigned ch, const rect_t& rc) const
{
    assert(hasItem_(plt));

    auto iconPos = rc.lower();
    iconPos.y() += (rc.height() - iconSize_.y()) * 0.5;
    paint->setColor(plt->majorColor(ch));
    paint->fillRect(iconPos, iconPos + iconSize_);

    auto lablePos = rc.lower();
    lablePos.x() += iconSize_.x() + iconTextPadding_;
    lablePos.y() += rc.height() * 0.5;
    paint->setColor(clrText_);
    paint->apply(fontText_);
    paint->drawText(lablePos, itemLabel_(plt, ch).c_str(), KeAlignment::k_left | KeAlignment::k_vcenter);
}


KcLegend::aabb_t KcLegend::boundingBox() const
{
    auto& rc = outterRect();
    return { 
        { rc.lower().x(), rc.lower().y(), 0 }, 
        { rc.upper().x(), rc.upper().y(), 0 } 
    };
}


unsigned KcLegend::itemCount() const
{
    unsigned count(0);
    for (auto i : plts_) 
        if (hasItem_(i)) // 忽略隐藏的plt和colorbar类型plt
            count += i->majorColors();
    return count;
}


std::string KcLegend::itemLabel_(KvPlottable* plt, unsigned ch) const
{
    assert(plt->majorColors() == plt->majorColorsNeeded());
    assert(ch < plt->majorColors());

    auto label = plt->name();
    if (plt->majorColors() > 1)
        label += "-ch" + KuStrUtil::toString(ch);
    return label;
}


unsigned KcLegend::nextVisiblePlt_(unsigned idx) const
{
    while (idx < plts_.size() && !hasItem_(plts_[idx]))
        ++idx;
    return idx;
}


void KcLegend::drawItems_(KvPaint* paint) const
{
    auto itemSize = maxLabelSize_(paint);
    itemSize.x() += iconSize_.x() + iconTextPadding_;
    itemSize.y() = std::max(itemSize.y(), iconSize_.y());

    auto lay = layouts_();
    assert(lay.x() > 0 && lay.y() > 0);

    auto itemPos = iRect_.lower() + innerMargins_.lower();
    unsigned rowStride = lay.y(), colStride = lay.x();

    rect_t itemRect(itemPos, itemPos + itemSize);
    unsigned pltIdx(nextVisiblePlt_(0)), chIdx(0); // 当前正在绘制的plt及其channel索引

    const int idim = rowMajor_ ? 1 : 0; // 内侧循环维度
    for (unsigned c = 0; c < lay[!idim]; c++) {

        if (pltIdx >= plts_.size()) break;

        auto rcNow = itemRect;
        for (unsigned r = 0; r < lay[idim]; r++) { // 先填充行

            if (pltIdx >= plts_.size()) break;

            drawItem_(paint, plts_[pltIdx], chIdx++, rcNow);

            rcNow.lower()[!idim] += itemSize[!idim] + itemSpacing_[!idim];
            rcNow.upper()[!idim] += itemSize[!idim] + itemSpacing_[!idim];

            if (chIdx >= plts_[pltIdx]->majorColorsNeeded()) {
                chIdx = 0;
                pltIdx = nextVisiblePlt_(++pltIdx);
            }
        }

        itemRect.lower()[idim] += itemSize[idim] + itemSpacing_[idim];
        itemRect.upper()[idim] += itemSize[idim] + itemSpacing_[idim];
    }
}


bool KcLegend::hasItem_(KvPlottable* plt) const
{
    return plt->visible() && plt->majorColorsNeeded() != -1;
}
