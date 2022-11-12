#pragma once
#include <optional>
#include "KtPoint.h"


// 二维直线线段

template<class KReal>
class KtLineS2d
{
	using point2 = KtPoint<KReal, 2>;

public:
	KtLineS2d(const point2& pt0, const point2& pt1)
		: pt0_(pt0), pt1_(pt1) {}

	
	// 求两条线段的交点
	std::optional<point2> intersects(const KtLineS2d& ls) const;

private:
	point2 pt0_, pt1_; // 两点构成线段
};


template<class KReal> std::optional<typename KtLineS2d<KReal>::point2> 
	KtLineS2d<KReal>::intersects(const KtLineS2d& ls) const
{
	auto B = pt1_ - pt0_;
	auto C = ls.pt0_ - pt0_;
	auto D = ls.pt1_ - pt0_;

	auto ABlen = B.squaredLength();
	if (ABlen <= 0) 
		return {};

	auto Bn = B / ABlen;
	C = point2(C.x() * Bn.x() + C.y() * Bn.y(), C.y() * Bn.x() - C.x() * Bn.y());
	D = point2(D.x() * Bn.x() + D.y() * Bn.y(), D.y() * Bn.x() - D.x() * Bn.y());

	if ((C.y() < 0 && D.y() < 0) || (C.y() >= 0 && D.y() >= 0))
		return {};

	auto ABpos = D.x() + (C.x() - D.x()) * D.y() / (D.y() - C.y());

	// Fail if segment C-D crosses line A-B outside of segment A-B.
	if ((ABpos < 0) || (ABpos > 1))
		return {};

	// (4) Apply the discovered position to line A-B in the original coordinate system.
	return pt0_ + B * ABpos;
}