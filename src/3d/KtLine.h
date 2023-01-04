#pragma once
#include "KtVector3.h"

// ֱ�ߣ�L(t) = B + tM
// @RAY: true��ʾΪ���ߣ���lineֻ��dir_������չ
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

	KReal distanceTo(const point3& pt) { return std::sqrt(squaredDistanceTo(pt)); } // ���߼�ľ���
	KReal squaredDistanceTo(const point3& pt); // ���߼�ľ���ƽ��

	KReal distanceTo(const KtLine& l) { return std::sqrt(squaredDistanceTo(l)); } // ���߼�ľ���
	KReal squaredDistanceTo(const KtLine& l); // ���߼�ľ���ƽ��

	// �жϵ�ptλ��ֱ�ߵ��Ĳ�
	// ����point(0, 0, 0)��ʾ��ֱ����
	// point.x��ֵ����pt��y-zƽ���ϣ�λ��ֱ�ߵ��Ĳ�
	// point.y��ֵ����pt��x-zƽ���ϣ�λ��ֱ�ߵ��Ĳ�
	// point.z��ֵ����pt��x-yƽ���ϣ�λ��ֱ�ߵ��Ĳ�
	point3 whichSide(const point3& pt) const {
		return dir_.cross(pt - point_);
	}


protected:
	point3 point_; // ֱ���ϵ�һ��
	vec3 dir_; // ֱ�ߵķ���ʸ��
};


// ��P��ֱ��L(t)=B+tM֮��ľ���D = |P-(B+t0M)|
// ����t0 = M(P-B)/(MM)
template<class KReal, bool RAY>
KReal KtLine<KReal, RAY>::squaredDistanceTo(const point3& pt)
{
	auto diff = pt - point_; // P-B
	auto t0 = diff.dot(dir_); // M(P-B)
	if (t0 > 0)
		diff -= dir_ * t0; // P-B-t0M

	return diff.squaredLength(); // |P-B-t0M|^2
}


// ֱ��L0(s)=B0+sM0��ֱ��L1(t)=B1+tM1���������ľ���Ϊ��
// Q(s, t) = |L0(s) - L1(t)| = sqrt(a*s^2+2*b*s*t+c*t^2+2*d*s+2*e*t+f)
// ����a=M0M0, b=-M0M1, c=M1M1, d=M0(B0-B1), e=-M1(B0-B1), f=(B0-B1)(B0-B1)
// ֱ�߼�ľ���ΪQ(s, t)����Сֵ����Q(s, t)ȡƫ��������������У�
// (a*s+b*t+d, b*s+c*t+e) = (0, 0)
// ���������̵ã�
// s = (be-cd)/(ac-b^2), t = (bd-ae)/(ac-b^2), 
// ���ac-b^2 = 0, ����ֱ��ƽ�У���ʱ��ʸһ���Կ�ȡ��
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
	if (fDet > 1e-6) { // ����ƽ��
		KReal s = -d / a;
		return s * d + f; // as^2+2*d*s+f = s*(a*s+2*d)+f = s*d+f
	}

	KReal e = -l.dir().dot(diff);
	KReal fInvDet = 1.0f / fDet;
	KReal s = (b * e - c * d) * fInvDet;
	KReal t = (b * d - a * e) * fInvDet;

	return s * (a * s + 2 * b * t + 2 * d) + t * (c * t + 2 * e) + f;
}
