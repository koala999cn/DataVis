#pragma once
#include <array>


template<typename T, int DIM>
class KtPoint : public std::array<T, DIM>
{
private:
	static_assert(DIM >= 2);
	using super_ = std::array<T, DIM>;

public:

	using super_::super_;
	KtPoint(const T& val) {
		super_::fill(val);
	}

	constexpr static int dim() { return DIM; }

	const T& x() const { return super_::at(0); }
	T& x() { return super_::at(0); }

	const T& y() const { return super_::at(1); }
	T& y() { return super_::at(1); }

	const T& z() const { return super_::at(2); }
	T& z() { return super_::at(2); }

	const T& w() const { return super_::at(3); }
	T& w() { return super_::at(3); }

	const T& u() const { return super_::at(4); }
	T& u() { return super_::at(4); }

	const T& v() const { return super_::at(5); }
	T& v() { return super_::at(5); }
};


using pt2d = KtPoint<double, 2>;
using pt2f = KtPoint<float,  2>;
using pt3d = KtPoint<double, 3>;
using pt3f = KtPoint<float,  3>;
using pt4d = KtPoint<double, 4>;
using pt4f = KtPoint<float,  4>;
