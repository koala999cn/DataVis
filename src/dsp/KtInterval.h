#pragma once
#include <assert.h>
#include <cmath>


// 数值区间的抽象
template<typename T>
class KtInterval
{
public:

    KtInterval() : start_(0), end_(0) {}

    KtInterval(T start, T end) : start_(start), end_(end) {
        assert(start_ <= end_);
    }

    KtInterval(const KtInterval& other)
        : start_(other.start())
        , end_(other.end()) {
    }


    void reset(T start, T end) {
        assert(start <= end);
        start_ = start, end_ = end;
    }

    void resetStart(T start) { start_ = start; }
    void resetEnd(T end) { end_ = end; }


    T start() const { return start_; }
    T end() const { return end_; }

    // 区间的长度
    T length() const { return end_ - start_; }


    // 判定是否为空区间
    bool empty() const {
        return start_ == end_;
    }


    KtInterval& operator=(const KtInterval& other) {
        start_ = other.start(), end_ == other.end();
    }


    bool operator==(const KtInterval& other) const {
        return start_ == other.start() && end_ == other.end();
    }

    bool operator!=(const KtInterval& other) const {
        return start_ != other.start() || end_ != other.end();
    }


    // 区间偏移offset
    void shift(T offset) {
        start_ += offset, end_ += offset;
    }

    void shiftStart(T offset) { start_ += offset; }
    void shiftEnd(T offset) { end_ += offset; }


    // 移动区间，以确保区间起点为newStart
    void shiftTo(T newStart) {
        end_ += newStart - start_;
        start_ = newStart;
    }


    // 移动区间，以确保区间起点为零点
    void shiftToSource() const {
        shiftTo(0);
    }

    // 区间缩放factor尺度
    void scale(T factor) {
        start_ *= factor, end_ *= factor;
    }


    // // 缩放区间，以确保区间长度为newLength
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
        shift(to.start());
    }


    // 判定x是否在本区间内
    bool cover(T x) const { return x >= start_ && x <= end_; }


    // 规整[x1, x2]为与本区间的交集
    // [x1, x2]区间可以是正序(x1>x2)，也可以是逆序(x1<x2)
    // 返回false表示[x1, x2]与本区间不相交
    bool intersect(T& x1, T& x2) const {
        if(x1 < x2) {
            if(x1 < start_) x1 = start_;   // intersect requested range with logical domain
            if(x2 > end_) x2 = end_;
            if(x2 <= x1) return false; // requested range and logical domain do not intersect
        }
        else {
            if(x2 < start_) x2 = start_;   // intersect requested range with logical domain
            if(x1 > end_) x1 = end_;
            if(x1 <= x2) return false; // requested range and logical domain do not intersect
        }
        assert(x1 != x2);
        return true;
    }


    /// 以下为处理超范围坐标的方式

    T fitNearest(T x) const {
        assert(!conver(x));
        return x > end() ? end() : start();
    }


    T fitMirror(T x) {
        assert(!conver(x));

        if(x > end()) {
            auto ratio = std::fmod(x - end(), length());
            return end() - ratio * length();
        }
        else {
            auto ratio = std::fmod(start() - x, length());
            return start() + ratio * length();
        }
    }

    T fitPeriod(T x) {
        assert(!conver(x));

        if(x > end()) {
            auto ratio = std::fmod(x - end(), length());
            return start() + ratio * length();
        }
        else {
            auto ratio = std::fmod(start() - x, length());
            return end() - ratio * length();
        }
    }

private:
    T start_, end_; // 数据所在区间
};
