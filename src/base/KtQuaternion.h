#pragma once
#include "KtVector3.h"
#include "KtVector4.h"
#include "KtMatrix3.h"
#include "KtuMath.h"
#include <assert.h>


// ��Ԫ����ʵ��
// �㷨�ο�<3D Game Engine Design - A Practical Approach To Real-Time Computer Graphics>

template<class KReal>
class KtQuaternion : protected KtPoint<KReal, 4>
{
	using super_ = KtPoint<KReal, 4>;
	using vec3 = KtVector3<KReal>;
	using vec4 = KtVector4<KReal>;
	template<bool ROW_MAJOR = true>
	using mat3 = KtMatrix3<KReal, ROW_MAJOR>;
	using kMath = KtuMath<KReal>;

public:

	using super_::super_;
	using super_::x;
	using super_::y;
	using super_::z;
	using super_::w;
	using super_::data;
	using super_::size;
	using super_::isApproxEqual;

	// �����㹹��
	KtQuaternion() : super_(0, 0, 0, 1) {}

	 /// Construct a quaternion from a rotation matrix
	template<bool ROW_MAJOR>
	KtQuaternion(const mat3<ROW_MAJOR>& rot);

	/// Construct a quaternion from an angle/axis
	KtQuaternion(KReal angle, const vec3& axis);

	// �Ӿֲ�����ϵ�����������ϵ��������������Ԫ��
	KtQuaternion(const vec3& x, const vec3& y, const vec3& z) 
	    : KtQuaternion(mat3<>(x, y, z)) {}

	/** Construct the shortest arc quaternion to rotate "from" vector to
		the destination "to" vector.
	*/
	KtQuaternion(const vec3& from, const vec3& to);

	static KtQuaternion zero() {
		return { 0, 0, 0, 0 };
	}

	static KtQuaternion identity() {
		return { 0, 0, 0, 1 };
	}

	bool isUnit() const {
		return kMath::almostEqual(norm(), 1);
	}

	// ��Ԫ���ĳ˷��ǲ��ɽ�����
	KtQuaternion operator*(const KtQuaternion& rhs) const;

	// ��ԭ����ϵ�е�ʸ��v�任��Ŀ������ϵ
	vec3 operator*(const vec3& v) const; 
 
	// ���ص�ǰ��Ԫ���Ĺ���
	KtQuaternion conjugate() const { 
		return KtQuaternion(-x(), -y(), -z(), w()); 
	} 

	// �Ե�ǰ��Ԫ�����й淶��
	KtQuaternion& normalize();

	KtQuaternion inverse() const;

	KtQuaternion unitInverse() const { return conjugate(); }

	KReal dot(const KtQuaternion& rhs) const { 
		return kMath::dot(data(), rhs.data(), size());
	} 

	// Squared-Length
	KReal norm() const { 
		return kMath::sum2(data(), size());
	} 

	// ����ʵ��
	KReal select() const { return w(); } 

	void toAngleAxis(KReal& angle, vec3& axis) const;

	template<bool ROW_MAJOR>
	void toRotateMatrix(mat3<ROW_MAJOR>& rot) const;

	void toAxes(vec3& x, vec3& y, vec3& z) const;

	vec3 xAxis() const; // ������ȡ�ֲ�����x����ĵ�λʸ�������������ͶӰ
	vec3 yAxis() const; // ������ȡ�ֲ�����y����ĵ�λʸ�������������ͶӰ
	vec3 zAxis() const; // ������ȡ�ֲ�����z����ĵ�λʸ�������������ͶӰ

	// ��ȡ��ǰ�ֲ�����ϵ�ĸ�������������Z��ת����ƫ����������Y��ת���͹������Ʒ�����X��ת����
	KReal pitch(); // ����
	KReal yaw(); // ƫ��
	KReal roll(); // ����

};


// cos(A) = (trace(R)-1)/2 
// 2cos^2(A/2) = 1+cos(A)  
// ==> w^2 = cos^2(A/2) = (trace(R)+1)/4
// ==> |w| = sqrt(trace(R)+1)/2
// ������������㣺
// 1. trace(R) > 0, |w| > 1/2
// w = sqrt(trace(R)+1)/2
// x = (r21-r12)/4w
// y = (r02-r20)/4w
// z = (r10-r01)/4w
// 2. trace(R) <= 0, |w| <= 1/2
//   2.1. r00ΪR��Խ�Ԫ�ص����ֵ
//        x = sqrt(r00-r11-r22+1)/2
//        w = (r21-r12)/4x
//        y = (r01+r10)/4x
//        z = (r02+r20)/4x
//  2.2. r11ΪR��Խ�Ԫ�ص����ֵ
//        y = sqrt(r11-r00-r22+1)/2
//        w = (r02-r20)/4y
//        x = (r01+r10)/4y
//        z = (r12+r21)/4y
//  2.3. r22ΪR��Խ�Ԫ�ص����ֵ
//        z = sqrt(r22-r00-r11+1)/2
//        w = (r10-r01)/4z
//        x = (r02+r20)/4z
//        y = (r12+r21)/4z
template<class KReal> template<bool ROW_MAJOR>
KtQuaternion<KReal>::KtQuaternion(const mat3<ROW_MAJOR>& rot)
{
	KReal trace = rot.m00() + rot.m11() + rot.m22();
	if (trace > 0) {
		w() = 0.5f * std::sqrt(trace + 1);
		KReal f = 0.25f / w();
		x() = f * (rot.m21() - rot.m12());
		y() = f * (rot.m02() - rot.m20());
		z() = f * (rot.m10() - rot.m01());
	}
	else if (rot.m00() > rot.m11() && rot.m00() > rot.m22()) {// r00Ϊ���ֵ
		x() = 0.5f * std::sqrt(rot.m00() - rot.m11() - rot.m22() + 1);
		KReal f = 0.25f / x();
		w() = f * (rot.m21() - rot.m12());
		y() = f * (rot.m10() + rot.m01());
		z() = f * (rot.m20() + rot.m02());
	}
	else if (rot.m11() > rot.m22()) { // m00 <= m11, r11Ϊ���ֵ
		y() = 0.5f * std::sqrt(rot.m11() - rot.m22() - rot.m00() + 1);
		KReal f = 0.25f / y();
		w() = f * (rot.m02() - rot.m20());
		x() = f * (rot.m10() + rot.m01());
		z() = f * (rot.m21() + rot.m12());
	}
	else {// r22Ϊ���ֵ
		z() = 0.5f * std::sqrt(rot.m22() - rot.m00() - rot.m11() + 1);
		KReal f = 0.25f / z();
		w() = f * (rot.m10() - rot.m01());
		x() = f * (rot.m20() + rot.m02());
		y() = f * (rot.m21() + rot.m12());
	}
}

// q = w+xi+yj+zk = cos(A/2)+sin(A/2)(u0i+u1j+u2k)
template<class KReal>
KtQuaternion<KReal>::KtQuaternion(KReal angle, const vec3& axis)
{
	assert(kMath::almostEqual(1, axis.length()));

	KReal halfAngle = angle / 2;
	KReal fsin = std::sin(halfAngle);
	w() = std::cos(halfAngle);
	x() = axis.x() * fsin;
	y() = axis.y() * fsin;
	z() = axis.z() * fsin;
}

// Based on Stan Melax's article in Game Programming Gems
template<class KReal>
KtQuaternion<KReal>::KtQuaternion(const vec3& from, const vec3& to)
{
	// Copy, since cannot modify local
	auto v0 = from.getNormalized();
	auto v1 = to.getNormalized();

	KReal d = v0.dot(v1);

	// If dot == 1, vectors are the same
	if (d >= 1.0f) {
		*this = identify();
	}
	if (d < (1e-6f - 1.0f)) {
		// ����������ת180�ȣ��˴�ѡ��from�Ĵ�ֱ��
		*this = KtQuaternion(kMath::pi, from.perpendicular());
	}
	else {
		KReal s = std::sqrt((1 + d) * 2);
		auto c = v0.cross(v1) / s;

		x() = c.x();
		y() = c.y();
		z() = c.z();
		w() = s * 0.5f;
		normalize();
	}
}


template<class KReal>
KtQuaternion<KReal>& KtQuaternion<KReal>::normalize()
{
	auto n = norm();
	if (n > 1e-8)
		operator *= 1 / std::sqrt(n);

	return *this;
}


template<class KReal>
KtQuaternion<KReal> KtQuaternion<KReal>::inverse() const
{
	auto n = norm();

	if (kMath::almostEqual(n, 0))
		return zero();

	auto invNorm = 1 / n;
	return KtQuaternion<KReal>(w * invNorm, -v * invNorm);
}

// ��λ��Ԫ��q = cos(A)+u*sin(A)������uΪ��ת�ᣬ2AΪ��ת�Ƕ�
// ==> A = 2*acos(w)
// ==> u = (x, y, z)/sqrt(1-w^2)
template<class KReal>
void KtQuaternion<KReal>::toAngleAxis(KReal& angle, vec3& axis) const
{
	assert(isUnit());

	if (kMath::almostEqual(1, std::abs(w()))) { // ��ת��Ϊ0����ת�������ѡ��ȡx��
		angle = 0.0f;
		axis = vec3::unitX();
	}
	else {
		assert(std::abs(w()) < 1);
		angle = 2 * std::acos(w);
		auto factor = 1 / kMath::sqrt(1 - w() * w()); // = 1.0f/sin(fAngle)
		axis.x = x() * factor;
		axis.y = y() * factor;
		axis.z = z() * factor;
	}
}

//     | ww+xx-yy-zz    2xy-2wz       2xz+2wy   |
// R = |   2xy+2wz    ww+yy-xx-zz     2yz-2wx   |
//     |   2xz-2wy       2yz+2wx    ww+zz-xx-yy |
// ���ڵ�λ��Ԫ�������Լ�Ϊ��
//     | 1-2yy-2zz    2xy-2wz     2xz+2wy  |
// R = |  2xy+2wz    1-2xx-2zz    2yz-2wx  |
//     |  2xz-2wy     2yz+2wx    1-2xx-2yy |
template<class KReal> template<bool ROW_MAJOR>
void KtQuaternion<KReal>::toRotateMatrix(mat3<ROW_MAJOR>& rot) const
{
	assert(isUnit()); // ֻ�Ե�λ��Ԫ����Ч

	KReal f2x = 2 * x();
	KReal f2y = 2 * y();
	KReal f2z = 2 * z();

	KReal f2xx = f2x * x();
	KReal f2yy = f2y * y();
	KReal f2zz = f2z * z();
	KReal f2xy = f2x * y();
	KReal f2xz = f2x * z();
	KReal f2yz = f2y * z();
	KReal f2wx = f2x * w();
	KReal f2wy = f2y * w();
	KReal f2wz = f2z * w();

	rot.m00() = 1 - f2yy - f2zz;
	rot.m10() = f2xy + f2wz;
	rot.m20() = f2xz - f2wy;
	rot.m01() = f2xy - f2wz;
	rot.m11() = 1 - f2xx - f2zz;
	rot.m21() = f2yz + f2wx;
	rot.m02() = f2xz + f2wy;
	rot.m12() = f2yz - f2wx;
	rot.m22() = 1 - f2xx - f2yy;
}

template<class KReal>
void KtQuaternion<KReal>::toAxes(vec3& x, vec3& y, vec3& z) const
{
	mat3<> rot;
	toRotateMatrix(rot);

	// ��ȡ��ת����ĸ���
	x = { rot.m00(), rot.m10(), rot.m20() };
	y = { rot.m01(), rot.m11(), rot.m21() };
	z = { rot.m02(), rot.m12(), rot.m22() };
}

template<class KReal>
KtVector3<KReal> KtQuaternion<KReal>::xAxis() const
{
	// 1-2yy-2zz, 2xy+2wz, 2xz-2wy
	assert(isUnit());

	KReal dy = 2 * y();
	KReal dz = 2 * z();

	return { 1 - dy * y() - dz * z(), dy * x() + dz * w(), dz * x() - dy * w() };
}

template<class KReal>
KtVector3<KReal> KtQuaternion<KReal>::yAxis() const
{
	// 2xy-2wz, 1-2xx-2zz, 2yz+2wx
	assert(isUnit());

	KReal dx = 2 * x();
	KReal dz = 2 * z();

	return { dx * y() - dz * w(), 1 - dx * x() - dz * z(), dz * y() + dx * w() };
}

template<class KReal>
KtVector3<KReal> KtQuaternion<KReal>::zAxis() const
{
	// 2xz+2wy, 2yz-2wx, 1-2xx-2yy
	assert(isUnit());

	KReal dx = 2 * x();
	KReal dy = 2 * y();

	return { dx * z() + dy * w(), dy * z() - dx * w(), 1 - dx * x() - dy * y() };
}

template<class KReal>
KtQuaternion<KReal> KtQuaternion<KReal>::operator*(const KtQuaternion<KReal>& rhs) const
{
	return {
		w() * rhs.x() + x() * rhs.w() + y() * rhs.z() - z() * rhs.y(),
		w() * rhs.y() - x() * rhs.z() + y() * rhs.w() + z() * rhs.x(),
		w() * rhs.z() + x() * rhs.y() - y() * rhs.x() + z() * rhs.w(),
		w() * rhs.w() - x() * rhs.x() - y() * rhs.y() - z() * rhs.z()
	};
}

template<class KReal>
KtVector3<KReal> KtQuaternion<KReal>::operator*(const vec3& v) const
{
	//	KtQuaternion<KReal> r(0, _v);
	//	return KtQuaternion<KReal>(*this*r*conjugate()).v; 

	// nVidia SDK implemention
	vec3 u{ x(), y(), z() };
	auto uv = u.cross(v);
	auto uuv = u.cross(uv);
	uv *= (2 * w());
	uuv *= 2;

	return v + uv + uuv;
}

// ��������X����ת
template<class KReal>
KReal KtQuaternion<KReal>::roll()
{
	// roll = atan2(localy.z, localy.y);

	KReal dx = 2 * v.x;
	KReal dz = 2 * v.z;

	return std::atan2(dz * y() + dx * w(), 1 - dx * x() - dz * z());
}

// ƫ������Z��ת��
template<class KReal>
KReal KtQuaternion<KReal>::yaw()
{
	// yaw = atan2(localx.y, localx.x);

	KReal dy = 2 * v.y;
	KReal dz = 2 * v.z;
 
	return std::atan2(dy * x() + dz * w(), 1 - dy * y() - dz * z());
}

// ��������Y��ת��
template<class KReal>
KReal KtQuaternion<KReal>::pitch()
{
	// pitch = atan2(localz.x, localz.z);

	KReal dx = 2 * x();
	KReal dy = 2 * y();

	return std::atan2(dx * z() + dy * w(), 1 - dx * x() - dy * y());
}

using quatf = KtQuaternion<float>;
using quatd = KtQuaternion<double>;

