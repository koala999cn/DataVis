#pragma once
#include <array>
#include <blitz/array.h>
#include "KtSampled.h"
#include "KuMath.h"
#include "KuDataUtil.h"


// 基于blitz++多维数组实现的采样数据
template<int DIM>
class KtSampledArray : public KtSampled<DIM>
{
public:
    using super_ = KtSampled<DIM>;

    KtSampledArray() = default;

    KtSampledArray(const KtSampledArray& other)
        : array_(other.array_), super_(other) {}

    KtSampledArray(const KvSampled& samp);

    /// 重载基类接口

    // 每个通道的数据总数
    kIndex size() const override {
        return channels() == 0 ? 0 : array_.size() / channels(); 
    }

    // 重载实现size，基类实现会有累计误差
    kIndex size(kIndex axis) const override {
        assert(axis < DIM);
        return array_.length(axis);
    }

    kIndex channels() const override {
        return array_.length(DIM);
    }

    void clear() override {
        super_::clear();
        auto shape = array_.shape();
        shape[0] = 0;
        array_.resizeAndPreserve(shape); 
    }

    void resize(kIndex shape[], kIndex chs = 0) override {
        super_::resize(shape);
        if (chs == 0)
            chs = channels();
        if (shape)
            array_.resizeAndPreserve(makeTinyVector_(shape, chs));
        else
            resizeChannel(chs);
    }

    using super_::value; // make helper-members visible

    kReal value(kIndex idx[], kIndex channel) const override {
        validateIndex(idx);
        return array_(makeTinyVector_(idx, channel));
    }

    // 更快速的实现
    kRange valueRange() const override {
        return array_.size() == 0 ? kRange(0, 0)
        : KuMath::minmax(array_.dataFirst(), array_.size());
    }


    /// 有关写操作的成员方法

    // 重置通道数
    void resizeChannel(kIndex c) {
        auto extent = array_.extent();
        extent[DIM] = c;
        array_.resizeAndPreserve(extent);
    }

    // 增加1个数据通道
    void addChannel() {
        resizeChannel(channels() + 1);
    }

    kReal* data() { return array_.dataFirst(); }
    const kReal* data() const { return array_.dataFirst(); }


    kReal* row(kIndex idx) { return &array_(int(idx)); }
    const kReal* row(kIndex idx) const { return &array_(int(idx)); }

    auto stride(kIndex axis) const { return array_.stride(axis); }

    // 直接从data拷贝数据填充底层数组
    void fill(const kReal* data) {
        std::copy(data, data + array_.size(), array_.dataFirst());
    }

    // 设置特定通道的数据
    // @idx: 大小等于DIM-1
    // @data: 设置的数据，长度等于size(DIM-1)
    void setChannel(kIndex idx[], kIndex channel, const kReal* data);

    // 从d的pos位置开始，附加frames帧数据到this
    // assert(d.step(0) == this->step(0) && d.channles() == this->channles())
    // @rows: 0表示从pos往后的所有帧
    void pushBack(const KtSampledArray& d, kIndex pos = 0, kIndex frames = 0);

    void pushBack(const KvSampled& d, kIndex pos = 0, kIndex frames = 0);

    // 增加frames帧数据，data的长度等于frames * channels()
    void pushBack(const kReal* data, kIndex frames);

    // 删除前rows行
    void popFront(kIndex rows);

    // 向this压入数据d，保持数据总行数等于totalRows
    // 若totalRows = 0，则保持当前行数总量不变
    void shift(const KvSampled& d, kIndex totalRows = 0);

    // 从第idx帧开始，提取frames帧（行）的数据到buf
    // @frames: 要提取的帧数，若等于0，表示提取idx之后的所有帧
    void extract(kIndex idx, kReal* buf, kIndex frames = 0) const;


    // 移除第0轴数值小于x的rows
    //void cutBefore(kReal x);

    kReal* at(kIndex idx[]);

    bool writable() const final { return true; }

    void write(kIndex idx[], kIndex channel, kReal val) override {
        *(at(idx) + channel) = val;
    }

private:
    static auto makeTinyVector_(kIndex idx[], kIndex channel) {
        blitz::TinyVector<int, DIM + 1> tv;
        std::copy(idx, idx + DIM, tv.begin());
        tv[DIM] = channel;
        return tv;
    }

private:
    blitz::Array <kReal, DIM + 1 > array_; // 增加1维度，以提供多通道支持
};



template<int DIM>
KtSampledArray<DIM>::KtSampledArray(const KvSampled& samp)
{
    kIndex shape[DIM];

    for (unsigned i = 0; i < DIM; i++) {
        reset(i, samp.range(i).low(), samp.step(i));
        shape[i] = samp.size(i);
    }

    shape[0] = 0;
    resize(shape, samp.channels());
    pushBack(samp); // copy the data
}


template<int DIM>
kReal* KtSampledArray<DIM>::at(kIndex idx[])
{
    validateIndex(idx);
    auto p = data();
    for (int i = 0; i < DIM; i++)
        p += idx[i] * stride(i);
    return p;
}


template<int DIM>
void KtSampledArray<DIM>::setChannel(kIndex idx[], kIndex channel, const kReal* data) {
    assert(channel >= 0 && channel < channels());

    blitz::TinyVector<int, DIM + 1> tv;
    if (DIM > 1) {
        assert(idx != nullptr);
        std::copy(idx, idx + DIM - 1, tv.begin());
    }

    tv[DIM - 1] = 0;
    tv[DIM] = channel;

    kReal* dst = &array_(tv);
    if (stride(DIM - 1) == 1) {
        std::copy(data, data + size(DIM - 1), dst);
    }
    else {
        for (kIndex i = 0; i < size(DIM - 1); i++) {
            *dst = *data++;
            dst += stride(DIM - 1);
        }
    }
}


template<int DIM>
void KtSampledArray<DIM>::pushBack(const KtSampledArray& d, kIndex pos, kIndex rows)
{
    assert(step(0) == d.step(0) && channels() == d.channels());
    if (rows <= 0 || rows > d.size(0) - pos)
        rows = d.size(0) - pos;

    pushBack(d.row(pos), rows);
}


template<int DIM>
void KtSampledArray<DIM>::pushBack(const KvSampled& d, kIndex pos, kIndex frames)
{
    assert(step(0) == d.step(0) && channels() == d.channels());
    if (frames <= 0 || frames > d.size(0) - pos)
        frames = d.size(0) - pos;

    auto shapeOld = array_.extent();
    std::vector<kIndex> shapeNew(DIM); // 变换到kIndex类型
    std::copy(shapeOld.begin(), shapeOld.end() - 1, shapeNew.begin()); // shape的最后值代表channel数，不拷贝
    auto offset = shapeOld[0];
    shapeNew[0] += frames; // 增加frames帧
    resize(shapeNew.data()); // 调整this存储布局，增加相应行数

    std::array<kIndex, DIM> idx;
    
    // 逐帧复制
    idx.fill(0);
    idx[0] = pos; // 初始化索引
    auto count = frames; // @count为要复制的帧数
    for (unsigned i = 1; i < DIM; i++)
        count *= d.size(i);

    auto shape = KuDataUtil::shape(d);
    for (kIndex i = 0; i < count; i++) {
        assert(idx.front() >= pos);

        idx[0] += offset - pos; // 变换到this索引
        auto buf = at(idx.data());
        idx[0] -= offset - pos; // 变换回d的索引
        for (kIndex ch = 0; ch < channels(); ch++) {
            *buf = d.value(idx.data(), ch);
            buf += stride(DIM); // 通道的stride
        }

        KuDataUtil::nextIndex(shape, idx.data());
        assert(idx.front() < d.size(0));
        if (idx.front() == 0) // 考虑进位
            idx.front() = pos;
    }
}

template<int DIM>
void KtSampledArray<DIM>::pushBack(const kReal* data, kIndex frames)
{
    auto shape = array_.extent();
    shape[0] += frames;
    kIndex dims[DIM];
    std::copy(shape.begin(), shape.end() - 1, dims);
    resize(dims);

    std::copy(data, data + stride(0) * frames, row(shape[0] - frames));
}


template<int DIM>
void KtSampledArray<DIM>::popFront(kIndex rows)
{
    std::copy(row(rows), array_.dataFirst() + array_.size(), array_.dataFirst());
    auto shape = array_.extent();
    shape[0] -= rows;
    array_.resizeAndPreserve(shape);
    samp_[0].cutHead(rows);
}


template<int DIM>
void KtSampledArray<DIM>::shift(const KvSampled& d, kIndex totalRows)
{
    assert(step(0) == d.step(0));
    assert(channels() == d.channels());
    for (unsigned i = 1; i < DIM; i++) 
        assert(size(i) == d.size(i));

    if (totalRows == 0)
        totalRows = size(0);

    if (d.size(0) >= totalRows) {
        clear();
        pushBack(d, d.size(0) - totalRows, totalRows);
    }
    else {
        if (d.size(0) + size(0) > totalRows) {
            auto x0 = samp_[0].x0();
            popFront(d.size(0) + size(0) - totalRows);
            alignX0(x0); // 保持x0不变
        }
        pushBack(d);
    }

    assert(size(0) <= totalRows);
}


template<int DIM>
void KtSampledArray<DIM>::extract(kIndex idx, kReal* buf, kIndex frames) const
{
    if (frames <= 0 || frames > size(0) - idx)
        frames = size(0) - frames;

    std::copy(row(idx), row(idx + frames), buf);
}


#if 0
template<int DIM>
void KtSampledArray<DIM>::cutBefore(kReal x)
{
    if (x <= samp_[0].low())
        return;

    auto nx = std::min(samp_[0].size(x - samp_[0].low()), size(0));
/*
    if (nx <= 0) {
        assert(empty() || point(&x, 0)[0] >= x);
    }
    else {
        assert(point(nx - 1)[0] < x);
        if (nx < size(0))
            assert(point(nx)[0] >= x);
    }*/

    if(nx > 0)
        popFront(nx);
}
#endif