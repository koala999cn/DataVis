#pragma once
#include "KtArray.h"


template<typename T, int DIM>
class KtColor : public KtArray<T, DIM>
{
private:
	static_assert(DIM == 3 || DIM == 4);
	using super_ = KtArray<T, DIM>;

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
