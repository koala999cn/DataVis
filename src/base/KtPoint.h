#pragma once
#include "KtArray.h"
#include "KtuMath.h"

template<typename T, int DIM>
class KtPoint : public KtArray<T, DIM>
{
private:
	static_assert(DIM >= 2);
	using super_ = KtArray<T, DIM>;
	using kMath = KtuMath<T>;

public:

	using super_::super_;
	using super_::data;
	using super_::size;

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
		for (unsigned i = 0; i < size(); i++)
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

	bool isNan() const {
		for (unsigned i = 0; i < size(); i++)
			if (std::isnan(at(i)))
				return true;
		return false;
	}

	bool isInf() const {
		for (unsigned i = 0; i < size(); i++)
			if (std::isinf(at(i)))
				return true;
		return false;
	}

	bool isDefined() const {
		return !isNan() && !isInf();
	}

	bool isZero() const {
		return *this == zero();
	}

	bool isApproxEqual(const KtPoint& rhs) const {
		for (unsigned i = 0; i < size(); i++) {
			if (!kMath::almostEqual(super_::at(i), rhs.at(i)))
				return false;
		}

		return true;
	}

	bool isApproxZero() const {
		return isApproxEqual(zero());
	}


	T squaredLength() const {
		T len(0);
		for (unsigned i = 0; i < size(); i++)
			len += super_::at(i) * super_::at(i);
		return len;
	}

	T length() const {
		return std::sqrt(squaredLength());
	}

	T abs() const {
		return length();
	}

	// make THIS normalized
	KtPoint& normalize() {
		*this = getNormalize();
		return *this;
	}

	KtPoint getNormalize() const {
		auto len = abs();
		return len == 0 ? *this : *this / len;
	}


	// 两点之间距离
	T squaredDistance(const KtPoint& rhs) const {
		T dist(0);
		for (unsigned i = 0; i < size(); i++)
			dist += (super_::at(i) - rhs.at(i)) * (super_::at(i) - rhs.at(i));
		return dist;
	}

	T distance(const KtPoint& rhs) const {
		return std::sqrt(squaredDistance(rhs));
	}
};


using point2d = KtPoint<double, 2>;
using point2f = KtPoint<float,  2>;
using point3d = KtPoint<double, 3>;
using point3f = KtPoint<float,  3>;
using point4d = KtPoint<double, 4>;
using point4f = KtPoint<float,  4>;
