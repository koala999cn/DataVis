#pragma once
#include <array>
#include "KtuMath.h"

template<typename T, int DIM>
class KtPoint : public std::array<T, DIM>
{
private:
	static_assert(DIM >= 2);
	using super_ = std::array<T, DIM>;
	using kMath = KtuMath<T>;

public:

	using super_::super_;
	using super_::data;
	using super_::size;

	// 所有元素都填充val值
	KtPoint(const T& val) { super_::fill(val); }

	template<typename... ARGS, 
		std::enable_if_t<sizeof...(ARGS) == DIM, bool> = true> // 加个enable_if, 否则上个构造多数情况下不会被调用
	KtPoint(ARGS... args) : super_{ static_cast<T>(args)... } {}

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

	/// 一般运算

	KtPoint& operator+() { return *this; }
	KtPoint operator-() const { 
		KtPoint pt;
		for (int i = 0; i < dim(); i++)
			pt[i] = -super_::at(i);
		return pt; 
	}

	KtPoint operator+(const KtPoint& rhs) const {
		KtPoint pt;
		kMath::add(data(), rhs.data(), pt.data(), size());
		return pt;
	}
	KtPoint operator-(const KtPoint& rhs) const {
		KtPoint pt;
		kMath::sub(data(), rhs.data(), pt.data(), size());
		return pt;
	}
	KtPoint operator*(const KtPoint& rhs) const {
		KtPoint pt;
		kMath::mul(data(), rhs.data(), pt.data(), size());
		return pt;
	}
	KtPoint operator/(const KtPoint& rhs) const {
		KtPoint pt;
		kMath::div(data(), rhs.data(), pt.data(), size());
		return pt;
	}

	KtPoint operator+(T dc) const {
		KtPoint pt;
		kMath::shift(data(), pt.data(), size(), dc);
		return pt;
	}
	KtPoint operator-(T dc) const {
		return operator+(-dc);
	}
	KtPoint operator*(T factor) const {
		KtPoint pt;
		kMath::scale(data(), pt.data(), size(), factor);
		return pt;
	}
	KtPoint operator/(T factor) const {
		return operator*(1/factor);
	}

	KtPoint& operator+=(const KtPoint& v) {
		kMath::add(data(), v.data(), data(), size());
		return *this;
	}
	KtPoint& operator-=(const KtPoint& v) {
		kMath::sub(data(), v.data(), data(), size());
		return *this;
	}
	KtPoint& operator*=(const KtPoint& v) {
		kMath::mul(data(), v.data(), data(), size());
		return *this;
	}
	KtPoint& operator/=(const KtPoint& v) {
		kMath::div(data(), v.data(), data(), size());
		return *this;
	}

	KtPoint& operator+=(T dc) {
		kMath::shift(data(), size(), dc);
		return *this;
	}
	KtPoint& operator-=(T dc) {
		return operator+=(-dc);
	}
	KtPoint& operator*=(T factor) {
		kMath::scale(data(), size(), factor);
		return *this;
	}
	KtPoint& operator/=(T factor) {
		return operator*=(1/ factor);
	}

	T length() const {
		T len(0);
		for (int i = 0; i < dim(); i++)
			len += super_::at(i) * super_::at(i);
		return std::sqrt(len);
	}

	bool isNan() const {
		return std::isnan(x()) || std::isnan(y()) || std::isnan(z());
	}

	bool isInf() const {
		return std::isinf(x()) || std::isinf(y()) || std::isinf(z());
	}

	bool isDefined() const {
		return !isNan() && !isInf();
	}


	static KtPoint ceil(const KtPoint& pt1, const KtPoint& pt2) {
		KtPoint pt;
		kMath::forEach(pt1.data(), pt2.data(), pt.data(), dim(), [](T x, T y) {
			return std::max(x, y);
			});
		return pt;
	}

	static KtPoint floor(const KtPoint& pt1, const KtPoint& pt2) {
		KtPoint pt;
		kMath::forEach(pt1.data(), pt2.data(), pt.data(), dim(), [](T x, T y) {
			return std::min(x, y);
			});
		return pt;
	}
};


using point2d = KtPoint<double, 2>;
using point2f = KtPoint<float,  2>;
using point3d = KtPoint<double, 3>;
using point3f = KtPoint<float,  3>;
using point4d = KtPoint<double, 4>;
using point4f = KtPoint<float,  4>;
