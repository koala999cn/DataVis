#include "KgPreprocess.h"
#include <memory>
#include "KtFraming.h"
#include "KtuMath.h"
#include "capture/KgPreemphasis.h"
#include "capture/KgWindowing.h"


namespace kPrivate
{
    struct KgPreprocessInternal_
    {
        std::unique_ptr<KtFraming<double>> framing;
        std::unique_ptr<KgPreemphasis> preemphasis;
        std::unique_ptr<KgWindowing> windowing;
    };
}


KgPreprocess::KgPreprocess(const KpOptions& opts)
    : opts_(opts)
{
    auto d = new kPrivate::KgPreprocessInternal_;
    d->framing = std::make_unique<KtFraming<double>>(opts_.frameSize, 1, opts_.frameShift);

    if (opts.preemphasis != 0.0)
        d->preemphasis = std::make_unique<KgPreemphasis>(opts_.frameSize, opts_.preemphasis);

    if (opts.windowType != KgWindowing::k_rectangle)
        d->windowing = std::make_unique<KgWindowing>(opts.frameSize, 
            KgWindowing::KeType(opts.windowType), opts.windowArg);

    dptr_ = d;
}


KgPreprocess::~KgPreprocess()
{
    delete (kPrivate::KgPreprocessInternal_*)dptr_;
}


void KgPreprocess::setHandler(std::function<void(double*, double)> h)
{
    handler_ = h;
}

void KgPreprocess::process(const double* buf, unsigned len) const
{
    auto d = (kPrivate::KgPreprocessInternal_*)dptr_;
    d->framing->apply(buf, buf + len, [this](const double* frame) {
        std::vector<double> out(odim());
        auto e = processOneFrame_(frame, out.data());
        handler_(out.data(), e);
        });
}


void KgPreprocess::flush() const
{
    auto d = (kPrivate::KgPreprocessInternal_*)dptr_;
    d->framing->flush([this](const double* frame) {
        std::vector<double> out(odim());
        auto e = processOneFrame_(frame, out.data());
        handler_(out.data(), e);
        });
}


double KgPreprocess::processOneFrame_(const double* in, double* out) const
{
    std::copy(in, in + odim(), out);

    if (opts_.dither != 0) {
        ; // TODO: DO dithering
    }

    if (opts_.removeDcOffset)
        KtuMath<double>::subMean(out, odim());

    double energy(0);
    if (opts_.useEnergy == k_use_energy_raw)
        energy = KtuMath<double>::sum2(out, odim());

    auto d = (kPrivate::KgPreprocessInternal_*)dptr_;

    if (d->preemphasis)
        d->preemphasis->porcess(out);

    if (d->windowing)
        d->windowing->porcess(out);

    if (opts_.useEnergy == k_use_energy_win)
        energy = KtuMath<double>::sum2(out, odim());

    return energy;
}