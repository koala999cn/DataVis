#include "KcSampled1d.h"
#include <assert.h>
#include <string.h>
#include "KtuMath.h"


void KcSampled1d::pushBack(const kReal* v, kIndex N)
{
    auto len = size(0);
    resize(len + N);
    std::copy(v, v + N * channels(), row(len));
}


void KcSampled1d::pushBack(const KcSampled1d& d, kIndex pos, kIndex nx)
{
    assert(step(0) == d.step(0) && channels() == d.channels());
    if (nx <= 0) nx = d.size() - pos;
    assert(pos + nx <= d.size());
    
    pushBack(d.row(pos), nx);
}


void KcSampled1d::extract(kIndex idx, kReal* buf, kIndex N) const
{
    assert(idx + N <= size());
    const kReal* data = row(idx);

    ::memcpy_s(buf, bytesOfSamples(N), data, bytesOfSamples(N));
}


void KcSampled1d::shiftLeftTo(kReal xlow)
{
    samp_[0].shiftLeftTo(xlow);
}


/*
KcSampled1d::KcSampled1d(const std::vector<kReal>& data, kIndex channels)
    : samp_(data.size())
    , data_(data)
    , channels_(channels)
{
    assert(data_.size() % channels == 0);
}


KcSampled1d::KcSampled1d(std::vector<kReal>&& data, kIndex channels)
    : samp_(data.size())
    , data_(std::move(data))
    , channels_(channels)
{
    assert(data_.size() % channels == 0);
}


KcSampled1d::KcSampled1d(const KcSampled1d& other)
    : samp_(other.samp_)
    , data_(other.data_)
    , channels_(other.channels_)
{

}


KcSampled1d::KcSampled1d(KcSampled1d&& other) 
    : samp_(other.samp_)
    , data_(std::move(other.data_))
    , channels_(other.channels_)
{

}


KcSampled1d::KcSampled1d(kReal dx, kReal x0_rel, kIndex channels)
    : samp_(0, 0, dx, dx * x0_rel)
    , channels_(channels)
{

}


kIndex KcSampled1d::size() const
{
    return channels() == 0 ? 0 : static_cast<kIndex>(data_.size()) / channels();
}


kIndex KcSampled1d::channels() const
{
    return channels_;
}


void KcSampled1d::reserve(kIndex size, kIndex channels)
{
    data_.reserve(size * channels);
}


KcSampled1d::kPoint2d KcSampled1d::value(kIndex idx, kIndex channel) const
{
    assert(channel < channels() && idx < size());
    return kPoint2d{ samp_.indexToX(idx),
                *(data_.begin() + offset_(idx, channel)) };
}


kReal KcSampled1d::y(kReal, kIndex) const
{
    assert(false); // TODO:
    return 0;
}


kRange KcSampled1d::range(kIndex axis) const
{
    return axis == 0 ? 
        kRange{ samp_.xmin(), samp_.xmax() } : 
        KtuMath<kReal>::minmax(data_.data(), data_.size());
}


void KcSampled1d::setSample(kIndex idx, kReal val, kIndex channel)
{
    assert(channel < channels() && idx < size());

    if(channel >= 0)
        *(data_.begin() + offset_(idx, channel)) = val;
    else
        for(kIndex c = 0; c < channels(); c++)
            *(data_.begin() + offset_(idx, c)) = val;
}


void KcSampled1d::setSamples(kIndex idx, const kReal* v, kIndex N)
{
    assert(idx + N <= size());
    kReal* p = data_.data() + offset_(idx);

    ::memcpy_s(p, bytesOfSamples(size()-idx), v, bytesOfSamples(N));
}


void KcSampled1d::getSamples(kIndex idx, kReal* v, kIndex N) const
{
    assert(idx + N <= size());
    const kReal* p = data_.data() + offset_(idx);

    ::memcpy_s(v, bytesOfSamples(N), p, bytesOfSamples(N));
}


void KcSampled1d::addSamples(const kReal* v, kIndex N)
{
    auto nc = channels() * static_cast<std::vector<kReal>::size_type>(N);
    data_.resize(data_.size() + nc); // 增加c组通道数据
    samp_.growTail(N); // 同步增大xmax
    std::copy(v, v + nc, data_.end() - nc);
}


void KcSampled1d::append(const KvData1d& d, kIndex pos, kIndex nx)
{
    assert(step(0) == d.step(0) && channels() == d.channels());
    if (nx <= 0) nx = d.size() - pos;
    assert(pos + nx <= d.size());
    reserve(size() + nx, channels_);
    
    auto sampled1d = dynamic_cast<const KcSampled1d*>(&d);
    if (sampled1d) { // 批量增加样本点
        addSamples(sampled1d->data() + sampled1d->offset_(pos), nx);
    }
    else { // 逐个增加样本点
        std::vector<kReal> samp(d.channels());
        for (kIndex i = pos; i < nx; i++) {
            for (kIndex channel = 0; channel < d.channels(); channel++) 
                samp[channel] = d.value(i, channel).y;
            
            addSamples(samp.data(), 1);
        }
    }
}


void KcSampled1d::copy(const KvData1d& d, kIndex pos, kIndex size)
{
    reset(d.step(0), d.channels());
    append(d, pos, size);
}


kIndex KcSampled1d::size(kIndex axis) const
{
    assert(axis == 0);
    assert(data_.size() == samp_.size());
    return data_.size();
}


kReal KcSampled1d::step(kIndex axis) const
{
    return axis == 0 ? samp_.dx() : k_nonuniform_step;
}


void KcSampled1d::clear()
{
    data_.clear();
    samp_.clear();
}


void KcSampled1d::reset(kReal dx, kIndex channels, kIndex nx)
{
     samp_.resetn(nx, dx, 0);
     channels_ = channels;
     data_.resize(static_cast<std::vector<kReal>::size_type>(nx) * channels);
}


void KcSampled1d::resize(kIndex nx, kIndex channels)
{
    data_.resize(nx * channels);
    samp_.resetn(nx, 0.5);
}


void KcSampled1d::setChannel(const kReal* data, kIndex channel)
{
    assert(channel < channels());

    if (channel == -1 || channels() == 1) {
        ::memcpy_s(data_.data(), bytesOfSamples(size()), data, bytesOfSamples(size()));
    }
    else {
        for (kIndex i = 0; i < size(); i++)
            setSample(i, *data++, channel);
    }
}



void KcSampled1d::popBack(kIndex n)
{
    assert(n <= size());
    data_.erase(data_.begin() + offset_(size() - n), data_.end());
    samp_.cutTail(n);
}


void KcSampled1d::cutBefore(kReal x)
{
    if (x <= samp_.xmin())
        return;

    auto nx = std::min(samp_.size(x - samp_.xmin()), size());
    if (nx <= 0) {
        assert(data_.empty() || value(0).x >= x);
    }
    else {
        assert(value(nx - 1).x < x);
        if (nx < size())
            assert(value(nx).x >= x);
    }

    popFront(nx);
}


void KcSampled1d::cutAfter(kReal x)
{
    if (x >= samp_.xmax())
        return;

    auto nx = std::min(samp_.size(samp_.xmax() - x), size());
    if (nx <= 0) {
        assert(data_.empty() || value(size() - 1).x < x);
    }
    else {
        assert(value(size() - nx).x >= x);
        if (nx < size())
            assert(value(size() - nx - 1).x < x);
    }

    popBack(nx);
}
*/