#include "KcAndrewsCurves.h"
#include "KvPaint.h"
#include "KvDiscreted.h"
#include "KtSampling.h"
#include "KcSampled1d.h"


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


unsigned KcAndrewsCurves::objectCount() const
{
	return discreted_()->size(0);
}


bool KcAndrewsCurves::objectVisible_(unsigned objIdx) const
{
	return true;
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


void* KcAndrewsCurves::drawObject_(KvPaint* paint, unsigned objIdx) const
{
	if (dataChanged() && objIdx == 0)
		const_cast<KcAndrewsCurves*>(this)->genCurves_();

	assert(objIdx < curves_->channels());

	auto samp1d = dynamic_cast<KcSampled1d*>(curves_.get());
	auto getter = [samp1d, objIdx, this](unsigned idx) -> std::vector<float_t> {
		auto pt = samp1d->pointAt(idx, objIdx);
		return { pt[0], pt[1], defaultZ(objIdx) };
	};

	return super_::drawObjectImpl_(paint, getter, samp1d->size(), objIdx);
}


KcAndrewsCurves::aabb_t KcAndrewsCurves::calcBoundingBox_() const
{
	// x���ֵ��ʼ��Ϊ[-pi, pi]
	aabb_t box(point3 (-KuMath::pi, -1, defaultZ(0)), 
		point3(KuMath::pi, 1, defaultZ(0)));
	
	// ����y��ֵ��
	if (!empty()) {

		auto disc = discreted_();
		if (disc->dim() == 1) { // ����������
			auto r0 = disc->valueRange(0);
			float_t sum(0);
			for (unsigned i = 1; i < data()->channels(); i++) {
				auto r = disc->valueRange(i);
				sum += KuMath::absMax(r.low(), r.high());
			}

			box.lower().y() = r0.low() / KuMath::sqrt2 - sum;
			box.upper().y() = r0.high() / KuMath::sqrt2 + sum;
		}
		else {
			if (curves_) { // �ø�͵���ķ�����ֱ����curves��ֵ���ͺ�1�����ڣ�
				auto r = curves_->valueRange();
				box.lower().y() = r.low();
				box.upper().y() = r.high();
			}
		}

		box.upper().z() = defaultZ(majorColorsNeeded() - 1); // ����z���ֵ
	}

	return box;
}


void KcAndrewsCurves::genCurves_()
{
	KtSampling<float_t> samp;
	samp.resetn(1024, -KuMath::pi, KuMath::pi, 0); // andrews���ߵĲ�������

	auto samp1d = new KcSampled1d;
	samp1d->resize(samp.size(), majorColors());
	samp1d->reset(0, -KuMath::pi, samp.dx());
	curves_.reset(samp1d);

	unsigned i(0); // andrews���ߵ����
	std::function<float_t(unsigned)> getter; 
	auto disc = discreted_();
	unsigned d = disc->dim();

	if (d == 1) { // һά���ݣ���ͨ��ΪAndrews�任��ά������
		kPrivate::KpAndrews andrews(disc->channels()); // ϵ����������ͨ����
		getter = [this, disc, andrews, &i](unsigned ix) {
			std::vector<float_t> x(disc->channels());
			for (unsigned ch = 0; ch < disc->channels(); ch++)
				x[ch] = disc->valueAt(i, ch);
			return andrews(x.data(), ix);
		};
	}
	else if (disc->isSampled() && d == 2) { // matrix����
		auto samp = std::dynamic_pointer_cast<const KvSampled>(disc);
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
		getter = [this, disc, andrews, &i, samp](unsigned ix) {
			std::vector<float_t> x = disc->pointAt(i % disc->size(0), i / disc->size(0));
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

