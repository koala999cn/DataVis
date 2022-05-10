#include "KuInterp1d.h"
#include <assert.h>
#include <vector>
#include "KtuMath.h"

using kMath = KtuMath<kReal>;


kReal KuInterp1d::quad(kReal x0, kReal y0, kReal x1, kReal y1, kReal x2, kReal y2, kReal x)
{
    assert(x1 != x0 && x2 != x1);

    kReal l1 = (x - x1) * (x - x2) / ((x0 - x1) * (x0 - x2));
    kReal l2 = (x - x0) * (x - x2) / ((x1 - x0) * (x1 - x2));
    kReal l3 = (x - x0) * (x - x1) / ((x2 - x0) * (x2 - x1));

    return y0 * l1 + y1 * l2 + y2 * l3; // TODO: 优化
}


kReal KuInterp1d::quad(kReal y0, kReal y1, kReal y2, kReal xidx)
{
    return quad(0, y0, 1, y1, 2, y2, xidx); // TODO: 优化
}


kReal KuInterp1d::hermite(kReal y0, kReal y1, kReal dy0, kReal dy1, kReal xphase)
{
    auto x = xphase;
    auto x2 = x * x;
    auto x3 = x2 * x;

    return y0 * (2 * x3 - 3 * x2 + 1) +
            y1 * (-2 * x3 + 3 * x2) +
            dy0 * (x3 - 2 * x2 + x) +
            dy1 * (x3 - x2);
}


// 使用Hermite算法，通过四点数据近似求解dy0, dy1
// 参考https://www.paulinternet.nl/?page=bicubic
kReal KuInterp1d::cubic(kReal y0, kReal y1, kReal y2, kReal y3, kReal xphase)
{
    // 将多项式展开后的快速实现
    kReal c = (y2 - y0) / 2;
    kReal v = y1 - y2;
    kReal w = c + v;
    kReal a = w + v + (y3 - y1) / 2;
    kReal b = w + a;

    auto y = (((a * xphase) - b) * xphase + c) * xphase + y1;
    assert(y == hermite(y1, y2, (y2-y0)/2, (y3-y1)/2, xphase));
    return y;
}


kReal KuInterp1d::poly(const kReal X[], const kReal Y[], kIndex n, kReal x)
{
    kReal y = 0;

    for (kIndex i = 0; i < n; i++) {
        kReal g = 1;  // accumulator
        for (kIndex j = 0; j < n; j++) {
            if (j != i)
                g *= (x - X[j]) / (X[i] - X[j]);
        }
        y += Y[i] * g;
    }

    return y;
}


kReal KuInterp1d::poly(const kReal Y[], kIndex n, kReal xidx, kIndex stride)
{
    kReal y = 0;

    for (kIndex i = 0; i < n; i++) {
        kReal g = 1;  // accumulator
        for (kIndex j = 0; j < n; j++) {
            if (j != i)
                g *= (xidx - j) / (i - j);
        }
        y += *Y * g;
        Y += stride;
    }

    return y;
}


kReal KuInterp1d::sinc(const kReal Y[], kIndex nx, kReal xidx, int depth)
{
    kIndex midleft = std::floor(xidx), midright = midleft + 1;

    if(nx < 1) return kMath::nan;
    if(xidx >= nx) return Y[nx-1];
    if(xidx < 0) return Y[0];
    if(xidx == midleft) return Y[midleft];

    /* 0 < x < nx && x not integer: interpolate. */
    if(depth > midright) depth = midright;
    if(depth > static_cast<int>(nx - midright)) depth = static_cast<int>(nx - midright);
    if(depth <= k_nearest)
        return Y[(kIndex)std::round(xidx)];
    if(depth == k_linear)
        return linear(Y[midleft], Y[midright], xidx - midleft);
    if(depth == k_cubic)
        return cubic(Y + midleft - 1, xidx - midleft);

    // do the actual sinc interpolate
    kIndex left = midright - depth, right = midleft + depth;
    kReal y = 0; // sinc插值结果
    kReal a = kMath::pi * (xidx - midleft);
    kReal halfsina = 0.5 * std::sin(a);
    kReal aa = a / (xidx - left + 1);
    kReal daa = kMath::pi / (xidx - left + 1);
    for(kIndex ix = midleft; ix >= left; ix--) {
        kReal d = halfsina / a * (1 + std::cos(aa));
        y += Y[ix] * d;
        a += kMath::pi;
        aa += daa;
        halfsina = - halfsina;
    }
    a = kMath::pi * (midright - xidx);
    halfsina = 0.5 * std::sin(a);
    aa = a / (right - xidx + 1);
    daa = kMath::pi / (right - xidx + 1);
    for(kIndex ix = midright; ix <= right; ix++) {
        auto d = halfsina / a * (1.0 + std::cos(aa));
        y += Y[ix] * d;
        a += kMath::pi;
        aa += daa;
        halfsina = - halfsina;
    }

    return y;
}


kReal KuInterp1d::neville(const kReal X[], const kReal Y[], kIndex n, kReal x, kReal* pdy)
{
    std::vector<kReal> c(n), d(n);
    kIndex ns = 0;
    auto dif = std::abs(x - X[0]);
    for (kIndex i = 0; i < n; i++) {
        auto dift = std::abs(x - X[i]);
        if (dift < dif) { // 这里用ns作为表中最近入口的指针
            ns = i;
            dif = dift;
        }
        c[i] = Y[i]; // 初始化表c和表d
        d[i] = Y[i];
    }
    auto y = Y[ns--]; // y的初始逼近
    kReal dy = 0;
    for (kIndex m = 1; m < n; m++) { // 对表中的每一列
        for (kIndex i = 0; i < n - m; i++) {
            auto ho = X[i] - x;
            auto hp = X[i + m] - x;
            auto w = c[i + 1] - d[i];
            auto den = ho - hp;
            assert(den != 0); // 仅当两个输入的x值（在误差范围内）相同时才发生
            den = w / den;
            d[i] = hp * den;
            c[i] = ho * den;
        }

        /*
          表中的一列处理完后，判断要把修正值c还是d加到y的累计值上，
          就是说，选择表中的哪一条路径——走上边的分叉还是下边的分叉。
          我们的做法是走表中最“直”的路径到达顶点，并相应地更新ns值
          以记录所走的路径。本方法求得目标点x上有偏估计值，最后用dy表示误差。
        */
        dy = (2 * (ns + 1) < (n - m) ? c[ns + 1] : d[ns--]);
        y += dy;
    }

    if (pdy) *pdy = dy;
    return y;
}


kReal KuInterp1d::rational(const kReal X[], const kReal Y[], kIndex n, kReal x, kReal* pdy)
{
    kIndex ns = 0;
    std::vector<kReal> c(n), d(n);
    auto hh = std::abs(x - X[0]);
    kReal y(0), dy(0);
    for (kIndex i = 0; i < n; i++) {
        auto h = std::abs(x - X[i]);
        if (h == 0) {
            y = Y[i];
            dy = 0;
            if (pdy != nullptr) *pdy = dy;
            return y;
        }
        else if (h < hh) {
            ns = i;
            hh = h;
        }
        c[i] = Y[i];
        d[i] = Y[i] + kMath::eps; // 防止0除0的情况
    }
    y = Y[ns--];
    for (kIndex m = 1; m < n; m++) {
        for (kIndex i = 0; i < n - m; i++) {
            auto w = c[i + 1] - d[i];
            auto h = X[i + m] - x;
            auto t = (X[i] - x) * d[i] / h;
            auto dd = t - c[i + 1];
            if (dd == 0) // 这种情况表明该0处存在极点
                return kMath::nan;
            dd = w / dd;
            d[i] = c[i + 1] * dd;
            c[i] = t * dd;
        }
        dy = 2 * (ns + 1) < (n - m) ? c[ns + 1] : d[ns--];
        y += dy;
    }
    if (pdy != nullptr) *pdy = dy;
    return y;
}

