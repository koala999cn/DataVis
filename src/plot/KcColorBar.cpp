#include "KcColorBar.h"
#include "KvPlottable.h"
#include "KvPaint.h"
#include "KtGradient.h"
#include "KvData.h"
#include "util/draw_gradient.h"


KcColorBar::KcColorBar(KvPlottable* plt)
    : KvRenderable(plt->name()) // TODO: name无法随plt动态更新
    , plt_(nullptr)
{
    assert(plt);

    align() = location_ = KeAlignment::k_right | KeAlignment::k_horz_first | KeAlignment::k_outter;
    
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
        name() = plt->name();

        auto r = plt->colorMappingRange();
        axis_->setRange(r.first, r.second);
    }
}


namespace kPrivate
{
    // @dim: 0表示在x方向上渐变色，1表示在y方向上渐变色
    // @dir: 1表示从低到高渐变色，-1表示从高到低渐变色
    void drawGradient(KvPaint* paint, const KtAABB<double, 2>& rect, 
        const KtGradient<float, color4f>& grad, int dim, int dir);
}


void KcColorBar::draw(KvPaint* paint) const
{
    assert(visible());

    paint->pushCoord(KvPaint::k_coord_screen);

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
    if (showBorder_) {
        paint->apply(border_);
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


KcColorBar::aabb_t KcColorBar::boundingBox() const
{
    assert(false);
    return {};
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


#include "KuMath.h"
namespace kPrivate
{
    void drawGradient(KvPaint* paint, const KtAABB<double, 2>& rect,
        const KtGradient<float, color4f>& grad, int dim, int dir)
    {
        auto lower = rect.lower();
        auto upper = rect.upper();
        auto width = upper[!dim] - lower[!dim]; // 非渐变维度的尺度
        auto length = upper[dim] - lower[dim]; // 渐变维度的尺度
        if (dir < 0) 
            std::swap(upper[dim], lower[dim]);

        typename KvPaint::color_t clrs[4];
        clrs[0] = clrs[1] = grad.at(0).second;
        typename KvPaint::point2 pts[4];
        pts[0] = pts[1] = pts[3] = lower; 
        pts[1][!dim] += width;
        pts[2] = pts[1];
        for (unsigned i = 1; i < grad.size(); i++) {
            auto& stop = grad.at(i);
            clrs[2] = clrs[3] = stop.second;
            pts[2][dim] = pts[3][dim] = KuMath::remap(stop.first, 0.f, 1.f, lower[dim], upper[dim]);
            paint->fillQuad(pts, clrs);

            clrs[0] = clrs[1] = clrs[2];
            pts[0] = pts[3], pts[1] = pts[2];
        }
    }
}