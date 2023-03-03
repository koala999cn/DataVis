#include "KcAndrewsCurves.h"
#include "KvPaint.h"
#include "KvDiscreted.h"
#include "KtSampling.h"
#include "KcSampled1d.h"


namespace kPrivate
{
	struct KpSinCosTable
	{
		KpSinCosTable(unsigned N) {
			sin_.resize(N);
			cos_.resize(N);

			KtSampling<double> samp; samp.resetn(N, -KuMath::pi, KuMath::pi, 0);
			for (unsigned i = 0; i < N; i++) {
				auto t = samp.indexToX(i);
				sin_[i] = std::sin(t);
				cos_[i] = std::cos(t);
			}
		}

		double sin(unsigned idx) const {
			return sin_[idx % sin_.size()];
		}

		double cos(unsigned idx) const {
			return cos_[idx % cos_.size()];
		}

		std::vector<double> sin_, cos_; // TODO: ��ʵ��1������
	};


	struct KpAndrews
	{
		// @coeffs: andrews������ϵ������
		KpAndrews(unsigned coeffs) {
			sincosPairs_ = (coeffs - 1) / 2;
			extraSin_ = (coeffs - 1) & 1; 
		}

		double operator()(double* x, unsigned it) const {
			//static KpSinCosTable table(1024);

			double fx = x[0] / KuMath::sqrt2; // ����

			KtSampling<double> samp; samp.resetn(1024, -KuMath::pi, KuMath::pi, 0);

			// �м��sin/cos�ԣ�
			// NB: ������table.sin(i * it)���sin(i * samp.indexToX(it))
			// ��Ҫ��Ϊ��i * samp.indexToX(it) = i * (-pi + it * dt) = -i*pi + i*it*dt
			// ����1��-i*pi��ƫ��
			unsigned i = 1;
			for (; i <= sincosPairs_; i++) 
				//fx += x[2 * i - 1] * table.sin(i * it) + x[2 * i] * table.cos(i * it);
				fx += x[2 * i - 1] * sin(i * samp.indexToX(it)) + x[2 * i] * cos(i * samp.indexToX(it));

			// ĩ�������sin
			if (extraSin_) 
				fx += x[2 * i - 1] * sin(i * samp.indexToX(it));

			return fx;
		}

	private:
		unsigned sincosPairs_;
		bool extraSin_;
	};
}


void KcAndrewsCurves::outputImpl_()
{
	KtSampling<float_t> samp;
	samp.resetn(1024, -KuMath::pi, KuMath::pi, 0); // andrews���ߵĲ�������

	auto in = std::dynamic_pointer_cast<const KvDiscreted>(idata());
	auto samp1d = new KcSampled1d;
	samp1d->resize(samp.size(), in->size(0));
	samp1d->reset(0, -KuMath::pi, samp.dx());
	curves_.reset(samp1d);

	unsigned i(0); // andrews���ߵ����
	std::function<float_t(unsigned)> getter; 
	unsigned d = in->dim();

	if (d == 1) { // һά���ݣ���ͨ��ΪAndrews�任��ά������
		kPrivate::KpAndrews andrews(in->channels()); // ϵ����������ͨ����
		getter = [this, in, andrews, &i](unsigned ix) {
			std::vector<float_t> x(in->channels());
			for (unsigned ch = 0; ch < in->channels(); ch++)
				x[ch] = in->valueAt(i, ch);
			return andrews(x.data(), ix);
		};
	}
	else if (in->isSampled() && d == 2) { // matrix����
		auto samp = std::dynamic_pointer_cast<const KvSampled>(in);
		kPrivate::KpAndrews andrews(samp->size(1)); // ϵ��������������
		getter = [this, andrews, &i, samp](unsigned ix) {
			std::vector<float_t> x(samp->size(1));
			for (unsigned j = 0; j < samp->size(1); j++)
				x[j] = samp->value(i % samp->size(0), j, i / samp->size(0));
			return andrews(x.data(), ix);
		};
	}
	else { // ��ά���ݣ�ֱ����ÿ����ά���ݵ㹹��andrews����
		kPrivate::KpAndrews andrews(d); // ϵ����������ά��
		getter = [this, in, andrews, &i, samp](unsigned ix) {
			std::vector<float_t> x = in->pointAt(i % in->size(0), i / in->size(0));
			return andrews(x.data(), ix);
		};
	}

	std::vector<float_t> buf(samp1d->size(0));
	for (; i < samp1d->channels(); i++) {
		for (unsigned j = 0; j < samp1d->size(0); j++)
			buf[j] = getter(j);
		samp1d->setChannel(nullptr, i, buf.data());
	}
}

