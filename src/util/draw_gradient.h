#pragma once
#include <functional>
#include "KuMath.h"
#include "KtPoint.h"
#include "plot/KtColor.h"
#include "plot/KtGradient.h"


// 绘制渐变色带的帮助函数
// @drawQuad: 用于绘制quad的functor
// @lower, @upper: 用于绘制色带的区域
// @grad: 待绘制的渐变色带，要求key值规范到[0, 1]区间
// @dim: 渐变的维度，0表示水平渐变，1表示垂直渐变
template<typename T1, typename T2>
void drawGradient(std::function<void(KtPoint<T1, 2>*, color4f*)> drawQuad, 
    const KtPoint<T1, 2>& lower, const KtPoint<T1, 2>& upper,
    const KtGradient<T2, color4f>& grad, int dim)
{
    if (grad.size() == 0)
        return;

    auto width = upper[!dim] - lower[!dim]; // 非渐变维度的尺度
    auto iter = grad.cbegin();

    KtPoint<T1, 2> pts[4];
    pts[0] = pts[1] = pts[3] = lower;
    pts[1][!dim] += width;
    pts[2] = pts[1];

    color4f clrs[4];
    clrs[0] = clrs[1] = iter->second;
    
    auto key_min = iter->first;
    auto key_max = std::prev(grad.cend())->first;
    assert(key_min >= 0 && key_max <= 1);

    if (key_min == 0)
        ++iter;

    for (; iter != grad.cend(); iter++) {
        clrs[2] = clrs[3] = iter->second;
        pts[2][dim] = pts[3][dim] = KuMath::remap(iter->first, 0.f, 1.f, lower[dim], upper[dim]);
        drawQuad(pts, clrs);

        clrs[0] = clrs[1] = clrs[2];
        pts[0] = pts[3], pts[1] = pts[2];
    }

    if (key_max != 1) {
        clrs[2] = clrs[3] = grad.map(1);
        pts[2][dim] = pts[3][dim] = upper[dim];
        drawQuad(pts, clrs);
    }
}