#pragma once
#include "KtPoint.h"


template<typename T, int DIM>
class KtColor : protected KtPoint<T, DIM>
{
private:
	static_assert(DIM == 3 || DIM == 4);
	using super_ = KtPoint<T, DIM>;

public:
	using super_::super_;

	const T& r() const { return super_::x(); }
	T& r() { return super_::x(); }

	const T& g() const { return super_::y(); }
	T& g() { return super_::y(); }

	const T& b() const { return super_::z(); }
	T& b() { return super_::z(); }

	const T& a() const { return super_::w(); }
	T& a() { return super_::w(); }

	template<std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	bool isValid() const {
		return *this == invalid();
	}

	// 返回无效的color对象
	template<std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
	static KtColor invalid() {
		return { -1 };
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
