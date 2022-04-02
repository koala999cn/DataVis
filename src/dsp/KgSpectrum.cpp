#include "KgSpectrum.h"
#include "KgRdft.h"
#include "KcSampled1d.h"
#include "KtuMath.h"


KgSpectrum::KgSpectrum()
	: df_(KvData::k_unknown_step)
	, nyquistFreq_(0)
	, type_(k_power)
	, floor_(1e-12f)
	, rdft_(nullptr)
{

}


KgSpectrum::~KgSpectrum()
{
	delete (KgRdft*)rdft_;
}


void KgSpectrum::reset(kReal dt, kIndex count)
{
	if (!rdft_ || count != countInTime()) {
		if (rdft_) delete (KgRdft*)rdft_;
		rdft_ = new KgRdft(count, false, true/*频谱乘以规一系数*/);
	}


	if (nyquistFreq_ != 1 / dt / 2) {
		nyquistFreq_ = 1 / dt / 2;
		KtSampling<kReal> samp;
		samp.resetn(countInFreq(), 0, nyquistFreq_, 0);
		df_ = samp.dx();
		// 此处没有采用df = 1/T的计算公式，主要原因有2：
		// 一是此处采用dt*count获取的T值不准确，大多数时候比真实的T值大1个dt；
		// 二是由此根据df推导出的频域range比nyquist频率大1-2个df。
		// 例如：当采样点为偶数N，频率采样点为N/2+1，此时计算的F' = df * (N/2+1) = F + 2*df 
	}
}


unsigned KgSpectrum::countInTime() const
{
	return rdft_ ? ((KgRdft*)rdft_)->sizeT() : 0;
}


unsigned KgSpectrum::countInFreq() const
{
	return rdft_ ? ((KgRdft*)rdft_)->sizeF() : 0;
}


void KgSpectrum::porcess(const KcSampled1d& samp, KcSampled1d& spec) const
{
	assert(rdft_ && countInTime() == samp.count());
	assert(samp.step(0) != KvData::k_nonuniform_step);

	spec.reset(df_, samp.channels(), countInFreq());

	// TODO: 优化单通道的情况
	std::vector<kReal> buf(samp.count());
	for (kIndex c = 0; c < samp.channels(); c++) {
		for (kIndex i = 0; i < samp.count(); i++)
			buf[i] = samp.value(i, c).y;

		porcess(buf.data());
		spec.setChannel(buf.data(), c);
	}
}


void KgSpectrum::porcess(kReal* data) const
{
	((KgRdft*)rdft_)->forward(data);
	((KgRdft*)rdft_)->powerSpectrum(data); // 功率谱

	// 转换为其他类型谱
	auto c = countInFreq();
	if (type_ == k_mag) {
		for (unsigned n = 0; n < c; n++)
			data[n] = sqrt(data[n]);
	}
	else if (type_ == k_log) {
		KtuMath<kReal>::applyFloor(data, c, floor_);
		KtuMath<kReal>::applyLog(data, c);
	}
	else if (type_ == k_db) {
		for (unsigned n = 0; n < c; n++) {
			data[n] = std::max(data[n], floor_);
			data[n] = 10 * log10(data[n]);
		}
	}
}
