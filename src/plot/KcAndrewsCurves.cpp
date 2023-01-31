#include "KcAndrewsCurves.h"
#include "KvDiscreted.h"
#include "KtSampling.h"


unsigned KcAndrewsCurves::majorColorsNeeded() const
{
	if (coloringMode() == k_colorbar_gradiant)
		return -1;

	if (empty())
		return 0;

	// NB: majorColorsNeeded����ֵ�����⹹����andrews���ߵ�����
	// Ϊ����������ܺ��������ݣ�����size(0)Ϊ���ߵ���Ŀ
	// -- ����һά���ݣ�����x�ᣬ�Ը�ͨ����y��������Ϊ����andrews���ߵ�ϵ��
	// -- ���ڸ�ά���ݣ������ݸ�ά����Ϊ����andrews���ߵ�ϵ��
	auto disc = discreted_();
	auto majors = disc->size(0);

	// NB: ���ڸ�ά���ݣ���ϸ�ͨ�����˷����޷����ֲ�ͬͨ�����ݣ�
	if (disc->dim() > 1)
		majors *= disc->channels(); 

	return majors;
}


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
			static KpSinCosTable table(1024);

			auto extraIdx = ((sincosPairs_ + 1) * it) % 1024;
			double fx = x[0] / KuMath::sqrt2 + extraSin_ ? x[2 * sincosPairs_ + 2] * table.sin_[extraIdx] : 0;
			for (unsigned i = 1; i <= sincosPairs_; i++) {
				auto idx = (i * it) % 1024;
				fx += x[2 * i] * table.sin_[idx] + x[2 * i + 1] * table.cos_[idx];
			}

			return fx;
		}

	private:
		unsigned sincosPairs_;
		bool extraSin_;
	};
}


void KcAndrewsCurves::drawDiscreted_(KvPaint* paint, KvDiscreted* disc) const
{
	unsigned i(0); // andrews���ߵ����

	KtSampling<float_t> samp;
	samp.resetn(1024, -KuMath::pi, KuMath::pi, 0); // andrews���ߵĲ�������

	GETTER getter; // �㷨�Ĺؼ����ڹ��촫�ݸ�drawImpl_��getter

	unsigned d = disc->dim();

	if (d == 1) { // һά���ݣ���ͨ��ΪAndrews�任��ά������
		kPrivate::KpAndrews andrews(disc->channels());
		getter = [this, disc, andrews, &i, samp](unsigned ix) -> std::vector<float_t> {
			std::vector<float_t> x(disc->channels());
			for (unsigned ch = 0; ch < disc->channels(); ch++)
				x[ch] = disc->valueAt(i, ch);

			return { samp.indexToX(ix), andrews(x.data(), ix), defaultZ() };
		};
	}
	else { // ��ά���ݣ�ֱ����ÿ����ά���ݵ㹹��andrews����
		kPrivate::KpAndrews andrews(d);
		getter = [this, disc, andrews, &i, samp](unsigned ix) -> std::vector<float_t> {
			std::vector<float_t> x = disc->pointAt(i % disc->size(0), i / disc->size(0));

			auto t = samp.indexToX(ix);
			return { t, andrews(x.data(),t), defaultZ() };
		};
	}

	for (; i < majorColorsNeeded(); i++) 
		drawImpl_(paint, getter, samp.size(), i);
}


KcAndrewsCurves::aabb_t KcAndrewsCurves::boundingBox() const
{
	// x���ֵ��ʼ��Ϊ[-pi, pi]
	aabb_t box(point3 (-KuMath::pi, -1, defaultZ(0)), 
		point3(KuMath::pi, 1, defaultZ(0)));
	
	// ����y��ֵ��
	if (!empty()) {

		if (data()->dim() == 1) {
			auto r0 = data()->valueRange(0);
			float_t sum(0);
			for (unsigned i = 1; i < data()->channels(); i++) {
				auto r = data()->valueRange(i);
				sum += KuMath::absMax(r.low(), r.high());
			}

			box.lower().y() = r0.low() / KuMath::sqrt2 - sum;
			box.upper().y() = r0.low() / KuMath::sqrt2 + sum;
		}
		else {

		}

		box.upper().z() = defaultZ(majorColorsNeeded() - 1); // ����z���ֵ
	}

	return box;
}