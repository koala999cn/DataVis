#pragma once
#include <cmath>
#include <limits>
#include "KtInterval.h"


// 数值采样的抽象
template<typename KREAL>
class KtSampling
{
public:

    KtSampling() : dx_(1), x0_(0) {}
    KtSampling(long nx) { resetn(nx); }

    KtSampling(KREAL xmin, KREAL xmax, KREAL dx, KREAL x0) {
        reset(xmin, xmax, dx, x0);
    }

    KtSampling(const KtSampling& samp)
        : x0_(samp.x0_)
        , dx_(samp.dx_)
        , interval_(samp.interval_) {}

    KtSampling& operator=(const KtSampling& samp) {
        x0_ = samp.x0_, dx_ = samp.dx_;
        interval_ = samp.interval_;
        return *this;
    }


    void reset(KREAL xmin, KREAL xmax, KREAL dx, KREAL x0) {
        interval_.reset(xmin, xmax);
        x0_ = x0;
        dx_ = dx;

        assert(interval_.cover(x0));
        assert(indexToX(nx()) > xmax && xToIndex(xmax) >= nx() - 1);
    }


    // 根据采样点数目nx重置本对象
    void resetn(long nx) {
        kReal xmax = nx - 1;
        if(nx == 0) xmax = 0;
        else if(nx == 1) xmax = 0.5;
        interval_.reset(0, xmax);
        x0_ = 0, dx_ = 1;

        assert(this->nx() == nx);
    }


    // 根据采样点数目nx和采样间隔dx重置本对象
    void resetn(long nx, KREAL dx) {
        interval_.reset(0, nx * dx);
        x0_ = dx/2, dx_ = dx;

        assert(this->nx() == nx);
    }


    // 根据采样点数目nx、采样区间[xmin, xmax]、x0相对偏移x0_rel_offset，重置本对象
    // @x0_rel_offset: 为0时，x0 = xmin; 为1时，x0 = xmin + dx; 为0.5时，x0 = xmin + dx/2; 等等
    void resetn(KREAL xmin, KREAL xmax, long nx, KREAL x0_rel_offset = 0) {
        assert(nx > 0);
        assert(xmax > xmin);
        assert(x0_rel_offset >= 0 && x0_rel_offset <= 1);

        interval_.reset(xmin, xmax);
        dx_ = (xmax - xmin) / static_cast<KREAL>(nx - 0.5 + x0_rel_offset);
        x0_ = xmin + x0_rel_offset * dx_;

        assert(this->nx() == nx && indexToX(nx) > xmax && xToIndex(xmax) >= nx - 1);
    }


    KREAL x0() const { return x0_; } // 首个采样点的采样时间
    KREAL dx() const { return dx_; } // 采样时间间隔
    KREAL xmin() const { return interval_.start(); }
    KREAL xmax() const { return interval_.end(); }
    KREAL duration() const { return interval_.length(); }
    KREAL rate() const { return 1/dx_; } // 采样率


    // 增长nx个采样点
    void growup(unsigned nx = 1) { interval_.shiftEnd(nx * dx_); }

    // 减少nx个采样点
    void cutdown(unsigned nx = 1) { interval_.shiftEnd(nx * -dx_); }


    // 计算第idx个采样点的采样时间
    KREAL indexToX(KREAL idx) const {
        return x0_ + idx * dx_;
    }

    // 返回采样时间t对应的采样点序号. 序号类型为KREAL，以方便后续执行插值操作
    KREAL xToIndex(KREAL x) const {
        return (x - x0_) / dx_;
    }

    long xToLowIndex(KREAL t) const {
        return static_cast<long>(std::floor(xToIndex(t)));
    }

    long xToHighIndex(KREAL t) const {
        return static_cast<long>(std::ceil(xToIndex(t)));
    }

    long xToNearestIndex(KREAL t) const {
        return static_cast<long>(std::round(xToIndex(t)));
    }


    // 通过this设定的参数推算采样点数量. 可用作一致性检查
    long nx() const {
        return xToLowIndex(xmax()) + 1;
    }


    // 计算区间[xl, xr]所对应的采样点序号
    std::pair<long, long> window(KREAL xl, KREAL xr) const {
        long idxLeft = xToHighIndex(xl);
        long idxRight = xToLowIndex(xr);
        return { idxLeft, idxRight };
    }


    // 计算区间[xl, xr]的采样点数量
    long nxInWindow(KREAL xl, KREAL xr) const {
        auto w = window(xl, xr);
        if(w.first > w.second) return 0;
        return w.second - w.first + 1;
    }


    void shift(KREAL offset) {
        x0_ += offset;
        interval_.shift(offset);
    }


    void scale(KREAL factor) {
        dx_ *= factor;
        auto offset = x0_ - xmin();
        x0_ = xmin() + offset * factor;
        interval_.scale(factor);   
    }


private:
    KREAL x0_, dx_;
    KtInterval<KREAL> interval_;
};


