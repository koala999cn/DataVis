#include "KcColorBar.h"
#include "KvPlottable.h"
#include "KvPaint.h"
#include "KtGradient.h"


KcColorBar::KcColorBar(KvPlottable* plt)
    : KvRenderable("ColorBar")
    , plt_(plt)
{
    align() = location_ = KeAlignment::k_right | KeAlignment::k_top | KeAlignment::k_outter;
    assert(plt_ != nullptr);
    margins_ = margins_t({ 5, 5 }, { 5, 5 });
}


namespace kPrivate
{
    // @dim: 0表示在x方向上渐变色，1表示在y方向上渐变色
    // @dir: 1表示从低到高渐变色，-1表示从高到低渐变色
    void drawGradient(KvPaint* paint, const KtAABB<double, 2>& rect, 
        const KtGradient<double, color4f>& grad, int dim, int dir);
}


void KcColorBar::draw(KvPaint* paint) const
{
    assert(visible());

    paint->pushCoord(KvPaint::k_coord_screen);

    auto box = innerRect();

    // draw gradient
    KtGradient<double, color4f> grad;
    for (unsigned i = 0; i < plt_->majorColors(); i++) 
        grad.setAt(double(i) / (plt_->majorColors() - 1), plt_->majorColor(i));

    if (location() & KeAlignment::k_horz_first) 
        kPrivate::drawGradient(paint, box, grad, 1, -1);
    else 
        kPrivate::drawGradient(paint, box, grad, 0, 1);

    // draw border
    if (showBorder_) {
        paint->setColor({ 0, 0, 0, 1 });
        paint->drawRect(box);
    }

    // TODO: draw the ticker

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


#include "KtuMath.h"
namespace kPrivate
{
    void drawGradient(KvPaint* paint, const KtAABB<double, 2>& rect,
        const KtGradient<double, color4f>& grad, int dim, int dir)
    {
        auto lower = rect.lower();
        auto upper = rect.upper();
        auto width = upper[!dim] - lower[!dim]; // 非渐变维度的尺度
        auto length = upper[dim] - lower[dim]; // 渐变维度的尺度
        if (dir < 0) 
            std::swap(upper[dim], lower[dim]);

        typename KvPaint::color_t clrs[4];
        clrs[0] = clrs[1] = grad.stopAt(0).second;
        typename KvPaint::point2 pts[4];
        pts[0] = pts[1] = pts[3] = lower; 
        pts[1][!dim] += width;
        pts[2] = pts[1];
        for (unsigned i = 1; i < grad.numStops(); i++) {
            auto& stop = grad.stopAt(i);
            clrs[2] = clrs[3] = stop.second;
            pts[2][dim] = pts[3][dim] = KtuMath<KvPaint::float_t>::remap(stop.first, 0, 1, lower[dim], upper[dim]);
            paint->fillQuad(pts, clrs);

            clrs[0] = clrs[1] = clrs[2];
            pts[0] = pts[3], pts[1] = pts[2];
        }
    }
}