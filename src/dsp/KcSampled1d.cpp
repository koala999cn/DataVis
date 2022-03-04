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


void KcSampled1d::reserve(kIndex size)
{
    data_.reserve(static_cast<size_t>(size) * static_cast<size_t>(channels()));
}


kPoint2d KcSampled1d::value(kIndex idx, kIndex channel) const
{
    assert(channel < channels() && idx < count());
    return kPoint2d{ samp_.indexToX(idx),
                *(data_.begin() + offset_(idx, channel)) };
}


kRange KcSampled1d::xrange() const
{
    return kRange{ samp_.xmin(), samp_.xmax() };
}


kRange KcSampled1d::yrange() const
{
    return KtuMath<kReal>::minmax(data_.data(), data_.size());
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


void KcSampled1d::addSamples(kReal* v, kIndex N)
{
    data_.resize(data_.size() + channels() * static_cast<std::vector<kReal>::size_type>(N)); // 增加c组通道数据
    samp_.growup(N); // 同步增大xmax
    kReal* p = data_.data() + offset_(count() - N);
    ::memcpy_s(p, bytesOfSamples(N), v, bytesOfSamples(N));
}


kReal KcSampled1d::step(int axis) const
{
    return axis == 0 ? samp_.dx() : k_nonuniform_step;
}


void KcSampled1d::clear()
{
    data_.clear();
    samp_.reset(samp_.xmin(), samp_.xmin(), samp_.dx(), samp_.x0());
}


void KcSampled1d::reset(kReal dx, kIndex channels, kIndex samples)
{
     samp_.resetn(samples, dx);
     channels_ = channels;
     data_.resize(static_cast<std::vector<kReal>::size_type>(samples) * channels);
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
