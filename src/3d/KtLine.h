#pragma once
#include "KtVector3.h"

// 直线：L(t) = B + tM
// @RAY: true表示为射线，即line只向dir_方向延展
template<class KReal, bool RAY = false>
class KtLine
{
	using point3 = KtPoint<KReal, 3>;
	using vec3 = KtVector3<KReal>;

public:
	KtLine() : point_(0), dir_(1) {
		dir_.normalize();
	}

	KtLine(const point3& point, const vec3& dir) : point_(point), dir_(dir) {
		dir_.normalize();
	}

	const point3& point() const { return point_; }
	point3& point() { return point_; }

	const vec3& dir() const { return dir_; }
	vec3& dir() { return dir_; }

	constexpr bool isRay() const { return RAY; }

	point3 pointAt(KReal t) { return point_ + dir_ * t; }

	KReal distanceTo(const point3& pt) { return std::sqrt(squaredDistanceTo(pt)); } // 点线间的距离
	KReal squaredDistanceTo(const point3& pt); // 点线间的距离平方

	KReal distanceTo(const KtLine& l) { return std::sqrt(squaredDistanceTo(l)); } // 线线间的距离
	KReal squaredDistanceTo(const KtLine& l); // 线线间的距离平方

	// 判断点pt位于直线的哪侧
	// 返回point(0, 0, 0)表示在直线上
	// point.x的值代表pt在y-z平面上，位于直线的哪侧
	// point.y的值代表pt在x-z平面上，位于直线的哪侧
	// point.z的值代表pt在x-y平面上，位于直线的哪侧
	point3 whichSide(const point3& pt) const {
		return dir_.cross(pt - point_);
	}


protected:
	point3 point_; // 直线上的一点
	vec3 dir_; // 直线的方向矢量
};


// 点P和直线L(t)=B+tM之间的距离D = |P-(B+t0M)|
// 其中t0 = M(P-B)/(MM)
template<class KReal, bool RAY>
KReal KtLine<KReal, RAY>::squaredDistanceTo(const point3& pt)
{
	auto diff = pt - point_; // P-B
	auto t0 = diff.dot(dir_); // M(P-B)
	if (t0 > 0)
		diff -= dir_ * t0; // P-B-t0M

	return diff.squaredLength(); // |P-B-t0M|^2
}


// 直线L0(s)=B0+sM0和直线L1(t)=B1+tM1任意两点间的距离为：
// Q(s, t) = |L0(s) - L1(t)| = sqrt(a*s^2+2*b*s*t+c*t^2+2*d*s+2*e*t+f)
// 其中a=M0M0, b=-M0M1, c=M1M1, d=M0(B0-B1), e=-M1(B0-B1), f=(B0-B1)(B0-B1)
// 直线间的距离为Q(s, t)的最小值，对Q(s, t)取偏导并令其等于零有：
// (a*s+b*t+d, b*s+c*t+e) = (0, 0)
// 解上述方程得：
// s = (be-cd)/(ac-b^2), t = (bd-ae)/(ac-b^2), 
// 如果ac-b^2 = 0, 两条直线平行，此时不矢一般性可取：
// s = -d/a, t = 0
template<class KReal, bool RAY>
KReal KtLine<KReal, RAY>::squaredDistanceTo(const KtLine<KReal, RAY>& l)
{
	auto diff = point_ - l.point();
	KReal a = dir().squaredLength();
	KReal b = -dir().dot(l.dir());
	KReal c = l.dir().squaredLength();
	KReal d = dir().dot(diff);
	KReal f = diff.squaredLength();

	KReal fDet = a * c - b * b;
	if (fDet > 1e-6) { // 两线平行
		KReal s = -d / a;
		return s * d + f; // as^2+2*d*s+f = s*(a*s+2*d)+f = s*d+f
	}

	KReal e = -l.dir().dot(diff);
	KReal fInvDet = 1.0f / fDet;
	KReal s = (b * e - c * d) * fInvDet;
	KReal t = (b * d - a * e) * fInvDet;

	return s * (a * s + 2 * b * t + 2 * d) + t * (c * t + 2 * e) + f;
}
