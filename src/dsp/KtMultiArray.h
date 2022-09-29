#pragma once
#include <blitz/array.h>

//
// 基于blitz的多维数组实现
// @T: 数组元素的类型
// @DIM: 数组的维度，必须>0
//

template<typename T, int DIM>
class KtMultiArray
{
    static_assert(DIM > 0);

public:
    KtArray() = default;

    // 每个通道的数据总数
    kIndex size() const override {
        return channels() == 0 ? 0 : array_.size() / channels();
    }

    auto size(int axis) const {
        return array_.length(axis);
    }

    void clear() {
        array_.resize(0); 
    }

    void resize(int shape[]) {
        array_.resizeAndPreserve(makeTinyVector_(shape, chs));
    }

    const T& at(int idx[]) const {
        return array_(makeTinyVector_(idx));
    }


    /// 有关写操作的成员方法

    T* data() { return array_.dataFirst(); }
    const T* data() const { return array_.dataFirst(); }

    T* row(int idx) { return &array_(idx); }
    const T* row(int idx) const { return &array_(idx); }

    auto stride(int axis) const { return array_.stride(axis); }

    // 直接从data拷贝数据填充底层数组
    void fill(const kReal * data) {
        std::copy(data, data + array_.size(), array_.dataFirst());
    }


    // 删除前rows行
    //void popFront(kIndex rows);

    // 移除第0轴数值小于x的rows
    //void cutBefore(kReal x);

private:
    static auto makeTinyVector_(int idx[]) {
        blitz::TinyVector<int, DIM> tv;
        std::copy(idx, idx + DIM, tv.begin());
        return tv;
    }

private:
    blitz::Array<T, DIM> array_;
};

