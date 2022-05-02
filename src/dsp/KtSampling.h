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
    using super_ = KtInterval<KREAL>;

    KtSampling() : super_{}, x0_{}, dx_{} {}

    // 构造函数是唯一个能设置x0绝对值的地方，其他reset系函数均采用相对值进行设置
    KtSampling(KREAL xmin, KREAL xmax, KREAL dx, KREAL x0) 
        : KtInterval(xmin, xmax) {
        dx_ = dx, x0_ = x0;
    }

    KtSampling& operator=(const KtSampling& rhs) {
        dx_ = rhs.dx_, x0_ = rhs.x0_;
        super_::reset(rhs.low(), rhs.high());
        return *this;
    }


    void reset(KREAL xmin, KREAL xmax, KREAL dx, KREAL x0_ref) {
        KtInterval::reset(xmin, xmax);
        x0_ = xmin + x0_ref * dx;
        dx_ = dx;

        assert(verify());
    }

    void resetn(long nx) {
        if (dx_ == 0) dx_ = 1;
        KtInterval::resetHigh(low() + nx * dx_);
    }

    // 根据采样点数目nx重置本对象
    // @x0_rel_offset: x0的相对偏移，取值[0, 1)，=0时，x0 = xmin; =0.5时，x0 = xmin + dx / 2; 等等
    void resetn(long nx, KREAL x0_rel_offset) {
        assert(x0_rel_offset >= 0 && x0_rel_offset < 1);

        resetn(nx);
        x0_ = low() + x0_rel_offset * dx_;

        assert(count() == nx);
        assert(verify());
    }


    // 根据采样点数目nx和采样间隔dx重置本对象
    void resetn(long nx, KREAL dx, KREAL x0_rel_offset) {
        dx_ = dx;
        resetn(nx, x0_rel_offset);
    }


    // 根据采样点数目nx、采样区间[xmin, xmax]、x0相对偏移x0_rel_offset，重置本对象
    void resetn(long nx, KREAL xmin, KREAL xmax, KREAL x0_rel_offset) {
        assert(nx > 0);
        assert(xmax > xmin);
        assert(x0_rel_offset >= 0 && x0_rel_offset < 1);

        dx_ = (xmax - xmin) / nx;
        resetLow(xmin);
        resetn(nx, x0_rel_offset);
    }

    void resample(KREAL dx) {
        assert(dx > 0);
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
        assert(dx_ != 0);
        return (x - x0_) / dx_;
    }

    long xToLowIndex(KREAL t) const {
        auto idx = std::floor(xToIndex(t));
        assert(indexToX(idx) <= t); 
        if (indexToX(idx + 1) <= t) // 处理0.9999999变为0的情况
            idx++;

        return idx;
    }

    long xToHighIndex(KREAL t) const {
        auto idx = std::ceil(xToIndex(t));
        assert(empty() || indexToX(idx) >= t); 
        if (indexToX(idx - 1) >= t) // 处理1.0000001变为2的情况
            idx--;
        return idx;
    }

    long xToNearestIndex(KREAL t) const {
        return std::round(xToIndex(t));
    }


    // 计算区间[xl, xr)所对应的采样点序号
    // 未对返回值作clamp处理，需要调用者自行判断返回值的合理性
    std::pair<long, long> rangeToIndex(KREAL xl, KREAL xr) const {
        auto left = xToLowIndex(xl);
        auto right = xToHighIndex(xr);
        return { left, right };
    }


    // 计算长度len的区间所包含的采样点数量
    long count(KREAL len) const {
        if (len == 0 || dx_ == 0)
            return 0;
        auto i = rangeToIndex(x0(), x0() + len);
        return i.second > i.first ? i.second - i.first : 0;
    }

    // 计算全区间的采样点数量
    long count() const {
        return count(length());
    }


    // 重新实现KtInterval的shift和scale系列操作，同步调整x0值 
    void shift(KREAL offset) {
        x0_ += offset;
        KtInterval::shift(offset);
    }

    void shiftLeftTo(KREAL newLow) {
        x0_ += newLow - low();
        KtInterval::shiftLeftTo(newLow);
    }

    void shiftRightTo(KREAL newHigh) {
        x0_ += newHigh - high();
        KtInterval::shiftRightTo(newHigh);
    }

    void extendLeft(KREAL offset) {
        x0_ += offset;
        KtInterval::extendLeft(offset);
    }

    void scale(KREAL factor) {
        dx_ *= factor;
        auto offset = x0_ - low();
        KtInterval::scale(factor);
        x0_ = low() + offset * factor;
    }

    void scaleTo(KREAL newLength) {
        assert(!empty());
        scale(newLength / length());
    }

    void scaleToUnit() {
        scaleTo(1);
    }

    void transform(const KtInterval& from, const KtInterval& to) {
        shiftLeftTo(0);
        scale(to.length() / from.length());
        shift(to.low());
    }


    bool verify() const {
        if (empty())
            return true;

        return indexToX(count()) >= high() 
            && xToIndex(high()) > (count() - 1) 
            && x0_ + (count() - 1) * dx_ < high()
            && x0_ + count() * dx_ >= high()
            && (empty() || cover(x0_));
    }


private:
    KREAL x0_, dx_;
};


