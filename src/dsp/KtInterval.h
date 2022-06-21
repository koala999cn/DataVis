#pragma once
#include <assert.h>
#include <cmath>


// 数值区间的抽象
template<typename T>
class KtInterval
{
public:

    KtInterval() = default;
    KtInterval(const KtInterval&) = default;
    KtInterval(KtInterval&&) = default;

    KtInterval& operator=(const KtInterval&) = default;
    KtInterval& operator=(KtInterval&&) = default;

    KtInterval(T low, T high) {
        assert(low <= high);
        low_ = low, high_ = high;
    }

    KtInterval(const std::pair<T, T>& p) {
        assert(p.first <= p.second);
        low_ = p.first, high_ = p.second;
    }


    void reset(T low, T high) {
        assert(low <= high);
        low_ = low, high_ = high;
    }

    void resetLow(T low) { 
        assert(low <= high_);
        low_ = low; 
    }

    void resetHigh(T high) { 
        assert(high >= low_);
        high_ = high; 
    }


    T low() const { return low_; }
    T high() const { return high_; }
    T mid() const { return (high_ - low_) / 2; }

    // 区间的长度
    T length() const { return high_ - low_; }


    // 判定是否为空区间
    bool empty() const {
        return low_ == high_;
    }


    bool operator==(const KtInterval& other) const {
        return low_ == other.low() && high_ == other.high();
    }

    bool operator!=(const KtInterval& other) const {
        return low_ != other.low() || high_ != other.high();
    }


    // 区间偏移offset，保持区间长度不变
    void shift(T offset) {
        low_ += offset, high_ += offset;
    }

    // 移动区间，以确保区间起点为newLow，保持区间长度不变
    void shiftLeftTo(T newLow) {
        high_ += newLow - low_;
        low_ = newLow;
    }

    // 移动区间，以确保区间终点为newHigh，保持区间长度不变
    void shiftRightTo(T newHigh) {
        low_ += newHigh - high_;
        high_ = newHigh;
    }

    // 以下变换不保持区间长度

    void extendLeft(T offset) { low_ += offset; }
    void extendRight(T offset) { high_ += offset; }


    // 区间缩放factor尺度
    void scale(T factor) {
        low_ *= factor, high_ *= factor;
    }


    // 缩放区间，以确保区间长度为newLength
    void scaleTo(T newLength) {
        assert(!empty());
        scale(newLength / length());
    }


    // 缩放区间，以确保区间长度为1
    void scaleToUnit() {
        scaleTo(1);
    }


    // 按照from到to的尺度，对本区间进行变换
    void transform(const KtInterval& from, const KtInterval& to) {
        shiftLeftTo(0);
        scale(to.length() / from.length());
        shift(to.low());
    }


    // 判定x是否在本区间内（左闭右开）
    bool cover(T x) const { return x >= low_ && x < high_; }


    // 规整[x1, x2]为与本区间的交集
    // [x1, x2]区间可以是正序(x1>x2)，也可以是逆序(x1<x2)
    // 返回false表示[x1, x2]与本区间不相交
    bool intersect(T& x1, T& x2) const {
        if(x1 < x2) {
            if(x1 < low_) x1 = low_;   // intersect requested range with logical domain
            if(x2 > high_) x2 = high_;
            if(x2 <= x1) return false; // requested range and logical domain do not intersect
        }
        else {
            if(x2 < low_) x2 = low_;   // intersect requested range with logical domain
            if(x1 > high_) x1 = high_;
            if(x1 <= x2) return false; // requested range and logical domain do not intersect
        }
        assert(x1 != x2);
        return true;
    }

    void makeFinite(T absMax) {
        assert(absMax > 0);
        if (std::isinf(low_))
            low_ = -absMax;
        if (std::isinf(high_))
            high_ = absMax;
    }

private:
    T low_, high_; // 数据所在区间
};
