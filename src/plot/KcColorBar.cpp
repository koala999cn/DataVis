#include "KcColorBar.h"
#include "KvPlottable.h"
#include "KvPaint.h"
#include "KtGradient.h"
#include "KvData.h"
#include "util/draw_gradient.h"


KcColorBar::KcColorBar(KvPlottable* plt)
    : super_(plt->name()) // TODO: name无法随plt动态更新
{
    assert(plt);

    align() = location() = KeAlignment::k_right | KeAlignment::k_horz_first | KeAlignment::k_outter;
    
    showBorder() = false; showBkgnd() = false;

    margins_ = margins_t({ 5, 5 }, { 5, 5 });

    axis_ = std::make_unique<KcAxis>(KcAxis::k_right, -1, false);
    axis_->showBaseline() = false;
    axis_->showSubtick() = false;

    resetPlottable(plt);
}


void KcColorBar::resetPlottable(KvPlottable* plt)
{
    if (plt != plt_) {
        plt_ = plt;
        setName(plt->name());

        auto r = plt->colorMappingRange();
        axis_->setRange(r.first, r.second);
    }
}


void KcColorBar::draw(KvPaint* paint) const
{
    assert(visible());

    paint->pushCoord(KvPaint::k_coord_screen);

    super_::draw(paint);

    auto box = innerRect();

    auto fillQuad = [paint](KtPoint<double, 2>* vtx, color4f* clr) {
        paint->fillQuad(vtx, clr);
    };

    auto flat = paint->flatShading();
    paint->enableFlatShading(false); // 禁用flat着色，否则不能正确绘制渐变色带
    if (align() & KeAlignment::k_horz_first) { // TODO: 更优雅的修正box的方法

        if (align() & KeAlignment::k_right)
            box.upper().x() = box.lower().x() + barWidth_;
        else if (align() & KeAlignment::k_left)
            box.lower().x() = box.upper().x() - barWidth_;

        // 反转y轴，确保由下往上渐变
        std::swap(box.lower().y(), box.upper().y());
        drawGradient<double, float>(fillQuad, box.lower(), box.upper(), plt_->gradient(), 1);
    }
    else {

        if (align() & KeAlignment::k_bottom)
            box.upper().y() = box.lower().y() + barWidth_;
        else if (align() & KeAlignment::k_top)
            box.lower().y() = box.upper().y() - barWidth_;

        drawGradient<double, float>(fillQuad, box.lower(), box.upper(), plt_->gradient(), 0);
    }
    paint->enableFlatShading(flat); // 恢复状态

    // draw border
    if (showBarBorder_) {
        paint->apply(barPen_);
        paint->drawRect(box);
    }

    // draw the ticker
    if (axis_->visible()) {
        typename KcAxis::point3 start, end;

        auto r = plt_->colorMappingRange();
        axis_->setRange(r.first, r.second);

        if (align() & KeAlignment::k_horz_first) {
            if (align() & KeAlignment::k_right) { // TODO: 更优雅的定位方法
                start = { box.upper().x(), box.lower().y(), 0 };
                end = { box.upper().x(), box.upper().y(), 0 };
            }
            else {
                start = { box.lower().x(), box.lower().y(), 0 };
                end = { box.lower().x(), box.upper().y(), 0 };
            }
        }
        else {
            if (align() & KeAlignment::k_bottom) {
                start = { box.lower().x(), box.upper().y(), 0 };
                end = { box.upper().x(), box.upper().y(), 0 };
            }
            else {
                start = { box.lower().x(), box.lower().y(), 0 };
                end = { box.upper().x(), box.lower().y(), 0 };
            }
        }

        axis_->setExtent(start, end);
        axis_->draw(paint);
    }

    paint->popCoord();
}


KcColorBar::size_t KcColorBar::calcSize_(void* cxt) const
{
    if (axis_->length() == 0 && plt_->odata()) {
        auto r = plt_->odata()->valueRange();
        axis_->setRange(r.low(), r.high());
    }

    if (location() & KeAlignment::k_horz_first) {
        axis_->setType(location() & KeAlignment::k_right ? KcAxis::k_right : KcAxis::k_left);
        axis_->setExtent(KcAxis::point3(0), KcAxis::point3(0, 1, 0));
    }
    else {
        axis_->setType(location() & KeAlignment::k_bottom ? KcAxis::k_bottom : KcAxis::k_top);
        axis_->setExtent(KcAxis::point3(0), KcAxis::point3(1, 0, 0));
    }

    KvPaint* paint = (KvPaint*)cxt;
    paint->pushCoord(KvPaint::k_coord_screen);       
    auto mar = axis_->calcMargins((KvPaint*)cxt);
    paint->popCoord();

    if (location() & KeAlignment::k_horz_first)
        return { barWidth_ + mar.left() + mar.right(), barLength_ };
    else 
        return { barLength_, barWidth_ + mar.top() + mar.bottom() };
}
