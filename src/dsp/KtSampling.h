#pragma once
#include <cmath>
#include <limits>
#include "KtInterval.h"


// 数值采样的抽象
// 采样区间左闭右开
template<typename KREAL>
class KtSampling : public KtInterval<KREAL>
{
public:

    KtSampling() = default;
    KtSampling(const KtSampling&) = default;
    KtSampling(KtSampling&&) = default;

    KtSampling& operator=(const KtSampling&) = default;
    KtSampling& operator=(KtSampling&&) = default;

    KtSampling(long nx) { resetn(nx, 0); }

    KtSampling(KREAL xmin, KREAL xmax, KREAL dx, KREAL x0) 
        : KtInterval(xmin, xmax) {
        dx_ = dx, x0_ = x0;
    }

    void reset(KREAL xmin, KREAL xmax, KREAL dx, KREAL x0_ref) {
        KtInterval::reset(xmin, xmax);
        x0_ = xmin + x0_ref * dx;
        dx_ = dx;

        //if (xmax > x0 + count() * dx)
        //    resetHigh(x0 + count() * dx);

        //assert(verify());
    }


    // 根据采样点数目nx重置本对象
    // @x0_rel_offset: x0的相对偏移，取值[0, 1)，=0时，x0 = xmin; =0.5时，x0 = xmin + dx / 2; 等等
    void resetn(long nx, KREAL x0_rel_offset) {
        assert(x0_rel_offset >= 0 && x0_rel_offset < 1);

        kReal xmax = nx;
        KtInterval::reset(0, xmax);
        x0_ = x0_rel_offset, dx_ = 1;

        //assert(count() == nx && verify());
    }


    // 根据采样点数目nx和采样间隔dx重置本对象
    void resetn(long nx, KREAL dx, KREAL x0_rel_offset) {
        assert(x0_rel_offset >= 0 && x0_rel_offset < 1);

        KtInterval::reset(0, nx * dx);
        x0_ = dx * x0_rel_offset, dx_ = dx;

        //assert(count() == nx && verify());
    }


    // 根据采样点数目nx、采样区间[xmin, xmax]、x0相对偏移x0_rel_offset，重置本对象
    void resetn(long nx, KREAL xmin, KREAL xmax, KREAL x0_rel_offset) {
        assert(nx > 0);
        assert(xmax > xmin);
        assert(x0_rel_offset >= 0 && x0_rel_offset < 1);

        KtInterval::reset(xmin, xmax);
        dx_ = (xmax - xmin) / nx;
        x0_ = xmin + x0_rel_offset * dx_;

        //if (xmax <= indexToX(nx - 1))
        //    dx_ = std::nextafter(dx_, xmin);
        
        //if (xmax > indexToX(nx))
        //    dx_ = std::nextafter(dx_, xmax);

        //assert(count() == nx && verify());
    }

    void resample(KREAL dx) {
        dx_ = dx;
    }

    void clear() {
        resetHigh(low()); // making high_ == low_
    }


    KREAL x0() const { return x0_; } // 首个采样点的采样时间
    KREAL x0ref() const { return (x0_ - xmin()) / dx_; }
    KREAL dx() const { return dx_; } // 采样时间间隔
    KREAL xmin() const { return low(); }
    KREAL xmax() const { return high(); }
    KREAL rate() const { return 1  /dx_; } // 采样率
    bool empty() const { return !cover(x0_); }

    // 增长nx个采样点
    void growTail(unsigned nx = 1) { extendRight(nx * dx_); }
    void growHead(unsigned nx = 1) { extendLeft(nx * -dx); }

    // 减少nx个采样点
    void cutTail(unsigned nx = 1) { extendRight(nx * -dx_); }
    void cutHead(unsigned nx = 1) { extendLeft(nx * dx_); }


    // 计算第idx个采样点的采样时间
    KREAL indexToX(KREAL idx) const {
        return x0_ + idx * dx_;
    }

    // 返回采样时间t对应的采样点序号. 序号类型为KREAL，以方便后续执行插值操作
    KREAL xToIndex(KREAL x) const {
        return (x - x0_) / dx_;
    }

    long xToLowIndex(KREAL t) const {
        auto idx = std::floor(xToIndex(t));
        if (indexToX(idx + 1) <= t) // 处理0.9999999变为0的情况
            idx++;

        if (idx < 0)
            idx = 0;
        return idx;
    }

    long xToHighIndex(KREAL t) const {
        auto idx = std::ceil(xToIndex(t));
        if (indexToX(idx) > t) // 处理1.0000001变为2的情况
            idx--;
        return idx;
    }

    long xToNearestIndex(KREAL t) const {
        return std::round(xToIndex(t));
    }


    // 计算区间[xl, xr)所对应的采样点序号
    // 未对返回值作clamp处理，需要调用者自行判断返回值的合理性
    std::pair<long, long> xRangeToIndex(KREAL xl, KREAL xr) const {
        auto left = xToLowIndex(xl);
        auto right = xToHighIndex(xr);
        return { left, right };
    }


    // 计算长度len的区间所包含的采样点数量
    long countLength(KREAL len) const {
        auto i = xRangeToIndex(low(), low() + len);
        return i.second - i.first;
    }

    // 计算全区间的采样点数量
    long count() const {
        return countLength(length());
    }

    void shift(KREAL offset) {
        x0_ += offset;
        KtInterval::shift(offset);
    }

    // TODO: 要对KtInterval其他的shift操作进行重载，确保x0值正确 

    void scale(KREAL factor) {
        dx_ *= factor;
        auto offset = x0_ - xmin();
        x0_ = xmin() + offset * factor;
        KtInterval::scale(factor);
    }


    bool verify() const {
        if (empty())
            return true;

        return indexToX(count()) >= high() 
            && xToIndex(high()) >= count() 
            && x0_ + (count() - 1) * dx_ < high()
            && x0_ + count() * dx_ >= high()
            && (empty() || cover(x0_));
    }


private:
    KREAL x0_, dx_;
};


