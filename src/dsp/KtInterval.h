#pragma once
#include <assert.h>
#include <cmath>


// 数值区间的抽象
template<typename T>
class KtInterval
{
public:

    KtInterval() : low_(0), high_(0) {}

    KtInterval(T low, T high) : low_(low), high_(high) {
        assert(low_ <= high_);
    }

    KtInterval(const std::pair<T, T>& p) : low_(p.first), high_(p.second) {
        assert(low_ <= high_);
    }

    KtInterval(const KtInterval& other)
        : low_(other.low())
        , high_(other.high()) {
    }


    void reset(T low, T high) {
        assert(low <= high);
        low_ = low, high_ = high;
    }

    void resetLow(T low) { low_ = low; }
    void resetHigh(T high) { high_ = high; }


    T low() const { return low_; }
    T high() const { return high_; }

    // 区间的长度
    T length() const { return high_ - low_; }


    // 判定是否为空区间
    bool empty() const {
        return low_ == high_;
    }


    KtInterval& operator=(const KtInterval& other) {
        low_ = other.low(), high_ == other.high();
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
    void shiftLowTo(T newLow) {
        high_ += newLow - low_;
        low_ = newLow;
    }

    // 移动区间，以确保区间终点为newHigh，保持区间长度不变
    void shiftHighTo(T newHigh) {
        low_ += newHigh - high_;
        high_ = newHigh;
    }

    // 以下变换不保持区间长度

    void shiftLow(T offset) { low_ += offset; }
    void shiftHigh(T offset) { high_ += offset; }


    // 区间缩放factor尺度
    void scale(T factor) {
        low_ *= factor, high_ *= factor;
    }


    // 缩放区间，以确保区间长度为newLength
    void scaleToLength(T newLength) {
        assert(!empty());
        scale(newLength / length());
    }


    // 缩放区间，以确保区间长度为1
    void scaleToUnitLength() {
        scaleToLength(1);
    }


    // 按照from到to的尺度，对本区间进行变换
    void transform(const KtInterval& from, const KtInterval& to) {
        shiftToSource();
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


    /// 以下为处理超范围坐标的方式

    T fitNearest(T x) const {
        assert(!conver(x));
        return x > high() ? high() : low();
    }


    T fitMirror(T x) {
        assert(!conver(x));

        if(x > high()) {
            auto ratio = std::fmod(x - high(), length());
            return high() - ratio * length();
        }
        else {
            auto ratio = std::fmod(low() - x, length());
            return low() + ratio * length();
        }
    }

    T fitPeriod(T x) {
        assert(!conver(x));

        if(x > high()) {
            auto ratio = std::fmod(x - high(), length());
            return low() + ratio * length();
        }
        else {
            auto ratio = std::fmod(low() - x, length());
            return high() - ratio * length();
        }
    }

private:
    T low_, high_; // 数据所在区间
};
