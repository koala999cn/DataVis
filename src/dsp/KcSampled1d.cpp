#include "KcSampled1d.h"
#include <assert.h>
#include <string.h>
#include "KtuMath.h"


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


kIndex KcSampled1d::count() const
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
    assert(channel < channels() && idx < count());
    return kPoint2d{ samp_.indexToX(idx),
                *(data_.begin() + offset_(idx, channel)) };
}


kRange KcSampled1d::range(kIndex axis) const
{
    return axis == 0 ? 
        kRange{ samp_.xmin(), samp_.xmax() } : 
        KtuMath<kReal>::minmax(data_.data(), data_.size());
}


void KcSampled1d::setSample(kIndex idx, kReal val, kIndex channel)
{
    assert(channel < channels() && idx < count());

    if(channel >= 0)
        *(data_.begin() + offset_(idx, channel)) = val;
    else
        for(kIndex c = 0; c < channels(); c++)
            *(data_.begin() + offset_(idx, c)) = val;
}


void KcSampled1d::setSamples(kIndex idx, const kReal* v, kIndex N)
{
    assert(idx + N <= count());
    kReal* p = data_.data() + offset_(idx);

    ::memcpy_s(p, bytesOfSamples(count()-idx), v, bytesOfSamples(N));
}


void KcSampled1d::getSamples(kIndex idx, kReal* v, kIndex N) const
{
    assert(idx + N <= count());
    const kReal* p = data_.data() + offset_(idx);

    ::memcpy_s(v, bytesOfSamples(N), p, bytesOfSamples(count() - idx));
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
    if (nx <= 0) nx = d.count() - pos;
    assert(pos + nx <= d.count());
    reserve(count() + nx, channels_);
    
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


void KcSampled1d::copy(const KvData1d& d, kIndex pos, kIndex count)
{
    reset(d.step(0), d.channels());
    append(d, pos, count);
}


kIndex KcSampled1d::length(kIndex axis) const
{
    assert(axis == 0);
    assert(data_.size() == samp_.count());
    return data_.size();
}


kReal KcSampled1d::step(kIndex axis) const
{
    return axis == 0 ? samp_.dx() : k_nonuniform_step;
}


void KcSampled1d::clear()
{
    data_.clear();
    samp_.reset(samp_.xmin(), samp_.xmin(), samp_.dx(), samp_.x0());
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
    samp_.resetn(nx, samp_.dx(), samp_.x0ref());
}


void KcSampled1d::setChannel(kReal* data, kIndex channel)
{
    assert(channel < channels());

    if (channel == -1 || channels() == 1) {
        ::memcpy_s(data_.data(), bytesOfSamples(count()), data, bytesOfSamples(count()));
    }
    else {
        for (kIndex i = 0; i < count(); i++)
            setSample(i, *data++, channel);
    }
}


void KcSampled1d::popFront(kIndex n)
{
    assert(n <= count());
    data_.erase(data_.begin(), data_.begin() + offset_(n));
    samp_.cutHead(n);
}


void KcSampled1d::popBack(kIndex n)
{
    assert(n <= count());
    data_.erase(data_.begin() + offset_(count() - n), data_.end());
    samp_.cutTail(n);
}


void KcSampled1d::cutBefore(kReal x)
{
    if (x <= samp_.xmin())
        return;

    auto nx = std::min(samp_.countLength(x - samp_.xmin()), count());
    if (nx <= 0) {
        assert(data_.empty() || value(0).x >= x);
    }
    else {
        assert(value(nx - 1).x < x);
        if (nx < count())
            assert(value(nx).x >= x);
    }

    popFront(nx);
}


void KcSampled1d::cutAfter(kReal x)
{
    if (x >= samp_.xmax())
        return;

    auto nx = std::min(samp_.countLength(samp_.xmax() - x), count());
    if (nx <= 0) {
        assert(data_.empty() || value(count() - 1).x < x);
    }
    else {
        assert(value(count() - nx).x >= x);
        if (nx < count())
            assert(value(count() - nx - 1).x < x);
    }

    popBack(nx);
}