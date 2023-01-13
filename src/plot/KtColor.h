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

	const T& r() const { return super_::at(0); }
	T& r() { return super_::at(0); }

	const T& g() const { return super_::at(1); }
	T& g() { return super_::at(1); }

	const T& b() const { return super_::at(2); }
	T& b() { return super_::at(2); }

	const T& a() const { return super_::at(3); }
	T& a() { return super_::at(3); }

	template<std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	bool isValid() const {
		return *this != invalid();
	}

	// 返回无效的color对象
	template<std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	static KtColor invalid() {
		return { -1, -1, -1, -1 };
	}

	// 返回元素的值域范围
	static constexpr std::pair<T, T> limits() {
		if constexpr (std::is_floating_point_v<T>)
			return { 0.f, 1.f };
		else
			return { std::numeric_limits<T>::min(), std::numeric_limits<T>::max() };
	}

	KtColor& clamp() {
		auto l = limits();
		for (int i = 0; i < DIM; i++) 
			at(i) = KtuMath<T>::clamp(at(i), l.first, l.second);
		return *this;
	}

	KtColor& brighten(T b) {
		for (int i = 0; i < 3; i++)
			at(i) += b; // 透明度不作运算
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
