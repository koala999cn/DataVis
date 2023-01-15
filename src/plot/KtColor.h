#pragma once
#include "KtPoint.h"


template<typename T, int DIM>
class KtColor : public KtPoint<T, DIM>
{
private:
	static_assert(DIM == 3 || DIM == 4);
	using super_ = KtPoint<T, DIM>;

public:
	using super_::super_;

	// ��ͬ������ɫ֮���ת��
	template<typename U, int E>
	KtColor(const KtColor<U, E>& clr) {
		for (int i = 0; i < std::min(DIM, E); i++)
			at(i) = static_cast<T>(double(clr.at(i)) * (double(limit()) / double(clr.limit())));
		if constexpr (DIM > E) {
			for (int i = E; i < DIM; i++)
				at(i) = limit();
		}
	}

	const T& r() const { return super_::at(0); }
	T& r() { return super_::at(0); }

	const T& g() const { return super_::at(1); }
	T& g() { return super_::at(1); }

	const T& b() const { return super_::at(2); }
	T& b() { return super_::at(2); }

	const T& a() const { return super_::at(3); }
	T& a() { return super_::at(3); }

	bool isValid() const {
		for (int i = 0; i < DIM; i++)
			if (at(i) < 0 || at(i) > limit())
				return false;

		return true;
	}

	// ������Ч��color����
	static KtColor invalid() {
		return { -1, -2, -3, -4 };
	}

	// ����Ԫ�ص����ֵ��Ĭ����СֵΪ0��
	static constexpr T limit() {
		if constexpr (std::is_floating_point_v<T>)
			return 1.f;
		else
			return std::numeric_limits<T>::max();
	}

	KtColor& clamp() {
		for (int i = 0; i < DIM; i++) 
			at(i) = KtuMath<T>::clamp(at(i), 0, limit());
		return *this;
	}

	KtColor& brighten(T b) {
		for (int i = 0; i < 3; i++)
			at(i) += b; // ͸���Ȳ�������
		clamp();
		return *this;
	}
};


using color3f = KtColor<float, 3>;
using color3d = KtColor<double, 3>;
using color3c = KtColor<unsigned char, 3>;
using color3i = KtColor<unsigned short, 3>;

using color4f = KtColor<float, 4>;
using color4d = KtColor<double, 4>;
using color4c = KtColor<unsigned char, 4>;
using color4i = KtColor<unsigned short, 4>;
