#pragma once
#include "kDsp.h"


// 数据插值: 给定若干个数据点(xi, yi)，求某个x对应的y值
// 当只给定若干个(yi)点时，xi缺省等于i
// 插值时，x有三种给值模式：
//    一是phase值，位于[0, 1]区间，相当于x/(xmax-xmin)
//    二是index值，位于[0, N-1]区间，相当于实数化的索引值，比如2.3，表示第3、第四数据点间，phase=0.3的x值
//    三是x值，即x的实际坐标值

class KuInterp1d
{
public:

    // x值与phase值的相互转换
    static kReal x2phase(kReal xmin, kReal xmax, kReal x) {
        return (x - xmin) / (xmax - xmin);
    }

    static kReal phase2x(kReal xmin, kReal xmax, kReal phase) {
        return xmin + phase * (xmax - xmin);
    }


    // 2点线性插值
    static kReal linear(kReal y0, kReal y1, kReal xpahse) {
        return y0 + xpahse * (y1 - y0);
    }

    static kReal linear(kReal x0, kReal y0, kReal x1, kReal y1, kReal x) {
        return (y1 - y0) * (x - x0) / (x1 - x0) + y0;
    }


    // 3点2次Lagrange插值
    // x为坐标值，位于[x0, x2]区间
    static kReal quad(kReal x0, kReal y0, kReal x1, kReal y1, kReal x2, kReal y2, kReal x);

    // xidx为索引值，位于[0, 2]区间，对应于[x0, x2]
    static kReal quad(kReal y0, kReal y1, kReal y2, kReal xidx);


    // 2点3次Hermite插值
    // dy0, dy1分别为y0, y1的导数
    // xphase为x的相位值，位于[0, 1]区间，对应于[x0, x1]
    static kReal hermite(kReal y0, kReal y1, kReal dy0, kReal dy1, kReal xphase);


    // 4点3次插值
    // xphase为x的相位值，位于[0, 1]区间，对应于[x1, x2]
    static kReal cubic(kReal y0, kReal y1, kReal y2, kReal y3, kReal xphase);

    // 数组版
    static kReal cubic(const kReal Y[], kReal xphase) {
        return cubic(Y[0], Y[1], Y[2], Y[3], xphase);
    }


    // Special values for sinc
    enum KeInterpDepth
    {
        k_nearest = 0,
        k_linear,
        k_cubic,
        // Higher values than 2 yield a true sinc interpolation
        k_sinc,
        k_sinc70 = 70,
        k_sinc700 = 700
    };

    // sinc插值，插值点数量为2*depth
    // depth可取KeInterpDepth值，也可直接用其他数字
    // depth<=0时，退化为nearest插值
    // depth=1时，退化为2点线性插值
    // depth=2时，退化为4点3次多项式插值
    // depth>2时，左右各取depth个点sinc插值
    // xidx为索引值，位于[0, nx)区间，对应于[ X[0], X[nx] )
    static kReal sinc(const kReal Y[], kIndex nx, kReal xidx, int depth);


    // Lagrange多项式插值
    // X[i]必须单调递增, 下同
    // x为坐标值，位于[X[0], X[nx-1]]区间，下同
    static kReal poly(const kReal X[], const kReal Y[], kIndex n, kReal x);

    // 均匀采样多项式插值
    static kReal poly(const kReal Y[], kIndex n, kReal xidx, kIndex stride = 1);

    // Neville多项式插值
    // 给定数组X[n]和Y[n]及数值x，返回x对应的y值及估计误差dy
    static kReal neville(const kReal X[], const kReal Y[], kIndex n, kReal x, kReal* dy = nullptr);


    // 对角有理函数插值
    // TODO: 该内插方法不平滑，得出的曲线毛刺较多
    static kReal rational(const kReal X[], const kReal Y[], kIndex n, kReal x, kReal* dy = nullptr);


private:
    KuInterp1d() { }
    ~KuInterp1d() { }
};

