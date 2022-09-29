#pragma once
#include "KtVector3.h"
#include "KtVector4.h"
#include "KtMatrix3.h"
#include "KtuMath.h"
#include <assert.h>


// 四元数的实现
// 算法参考<3D Game Engine Design - A Practical Approach To Real-Time Computer Graphics>

template<class KReal>
class KtQuaternion : protected KtPoint<KReal, 4>
{
	using super_ = KtArray<KReal, 4>;
	using vec3 = KtVector3<KReal>;
	using vec4 = KtVector4<KReal>;
	using mat3 = KtMatrix3<KReal>;
	using kMath = KtuMath<KReal>;

public:

	using super_::super_;
	using super_::x;
	using super_::y;
	using super_::z;
	using super_::w;
	using super_::data;
	using super_::size;
	using super_::dim;
	using super_::isApproxEqual;

	// 重载零构造
	KtQuaternion() : super_(0, 0, 0, 1) {}

	/*
	KtQuaternion(KReal x, KReal y, KReal z, KReal w) : super_(x, y, z, w) {}
	KtQuaternion(const vec4& v) : super_(v) {}
	KtQuaternion(const KReal data[]) : super_(data) {}*/

	 /// Construct a quaternion from a rotation matrix
	KtQuaternion(const mat3& rot) { 
		fromRotationMatrix(rot); 
	}

	/// Construct a quaternion from an angle/axis
	KtQuaternion(KReal angle, const vec3& axis) { 
		fromAngleAxis(angle, axis); 
	}

	static KtQuaternion zero() {
		return { 0, 0, 0, 0 };
	}

	static KtQuaternion identity() {
		return { 0, 0, 0, 1 };
	}

	bool isUnit() const {
		return kMath::almostEqual(norm(), 1);
	}

	KtQuaternion operator*(const KtQuaternion& rhs) const;

	vec3 operator*(const vec3& v) const; // 将原坐标系中的矢量v变换到目标坐标系
 
	// 返回当前四元数的共轭
	KtQuaternion conjugate() const { 
		return KtQuaternion(-x(), -y(), -z(), w()); 
	} 

	// 对当前四元数进行规范化
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

	// 返回实部
	KReal select() const { return w(); } 

	void toAngleAxis(KReal& angle, vec3& axis) const;
	void fromAngleAxis(KReal angle, const vec3& axis);
	void toRotationMatrix(mat3& rot) const;
	void fromRotationMatrix(const mat3& rot);

	void toAxes(vec3& x, vec3& y, vec3& z) const;
	vec3 getXAxis() const; // 单独提取局部坐标x方向的单位矢量在世界坐标的投影
	vec3 getYAxis() const; // 单独提取局部坐标y方向的单位矢量在世界坐标的投影
	vec3 getZAxis() const; // 单独提取局部坐标z方向的单位矢量在世界坐标的投影
	void formAxes(const vec3& x, const vec3& y, const vec3& z);

	// 获取当前局部坐标系的俯仰（绕右向轴Z旋转）、偏航（绕上轴Y旋转）和滚动（绕方向轴X旋转）角
	KReal pitch(); // 俯仰
	KReal yaw(); // 偏航
	KReal roll(); // 滚动
};

/** Gets the shortest arc quaternion to rotate kvFrom vector to the destination
    kvTo vector.
@remarks
    If you call this with a dest kvTo vector that is close to the inverse
    of kvFrom vector, we will rotate 180 degrees around the 'kvFallbackAxis'
	(if specified, or a generated axis if not) since in this case
	ANY axis of rotation is valid.
*/
// TODO：VS编译器在处理默认的模板参数时，会发生内部错误。
template<class KReal>
KtQuaternion<KReal> GetRotationBetween(const KtVector3<KReal>& kvFrom, const KtVector3<KReal>& kvTo, 
	const KtVector3<KReal>& kvFallbackAxis/* = KtVector3<KReal>::ZERO*/);



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
	return KtQuaternion<KReal>(w * fInvNorm, -v * fInvNorm);
}

// 单位四元数q = cos(A)+u*sin(A)，其中u为旋转轴，2A为旋转角度
// ==> A = 2*acos(w)
// ==> u = (x, y, z)/sqrt(1-w^2)
template<class KReal>
void KtQuaternion<KReal>::toAngleAxis(KReal& angle, vec3& axis) const
{
	assert(isUnit());

	if (kMath::almostEqual(1, std::abs(w()))) { // 旋转角为0，旋转轴可任意选择，取x轴
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

// q = w+xi+yj+zk = cos(A/2)+sin(A/2)(u0i+u1j+u2k)
template<class KReal>
void KtQuaternion<KReal>::fromAngleAxis(KReal angle, const vec3& axis)
{
	assert(kMath::almostEqual(1, axis.length()));

	KReal halfAngle = angle / 2;
	KReal fsin = std::sin(halfAngle);
	w() = std::cos(halfAngle);
	x() = axis.x() * fsin;
	y() = axis.y() * fsin;
	z() = axis.z() * fsin;
}

//     | ww+xx-yy-zz    2xy-2wz       2xz+2wy   |
// R = |   2xy+2wz    ww+yy-xx-zz     2yz-2wx   |
//     |   2xz-2wy       2yz+2wx    ww+zz-xx-yy |
// 对于单位四元数，可以简化为：
//     | 1-2yy-2zz    2xy-2wz     2xz+2wy  |
// R = |  2xy+2wz    1-2xx-2zz    2yz-2wx  |
//     |  2xz-2wy     2yz+2wx    1-2xx-2yy |
template<class KReal>
void KtQuaternion<KReal>::toRotationMatrix(mat3& rot) const
{
	assert(isUnit()); // 只对单位四元数有效

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

// cos(A) = (trace(R)-1)/2 
// 2cos^2(A/2) = 1+cos(A)  
// ==> w^2 = cos^2(A/2) = (trace(R)+1)/4
// ==> |w| = sqrt(trace(R)+1)/2
// 分两种情况计算：
// 1. trace(R) > 0, |w| > 1/2
// w = sqrt(trace(R)+1)/2
// x = (r21-r12)/4w
// y = (r02-r20)/4w
// z = (r10-r01)/4w
// 2. trace(R) <= 0, |w| <= 1/2
//   2.1. r00为R阵对角元素的最大值
//        x = sqrt(r00-r11-r22+1)/2
//        w = (r21-r12)/4x
//        y = (r01+r10)/4x
//        z = (r02+r20)/4x
//  2.2. r11为R阵对角元素的最大值
//        y = sqrt(r11-r00-r22+1)/2
//        w = (r02-r20)/4y
//        x = (r01+r10)/4y
//        z = (r12+r21)/4y
//  2.3. r22为R阵对角元素的最大值
//        z = sqrt(r22-r00-r11+1)/2
//        w = (r10-r01)/4z
//        x = (r02+r20)/4z
//        y = (r12+r21)/4z
template<class KReal>
void KtQuaternion<KReal>::fromRotationMatrix(const mat3& rot)
{
	KReal fTrace = rot.m00() + rot.m11() + rot.m22();
	if (fTrace > 0) {
		w = 0.5f * std::sqrt(fTrace + 1);
		KReal fCoeff = 0.25f / w;
		v.x = fCoeff * (rot.m21() - rot.m12());
		v.y = fCoeff * (rot.m02() - rot.m20());
		v.z = fCoeff * (rot.m10() - rot.m01());
	}
	else if (rot.m00() > rot.m11() && rot.m00() > rot.m22()) {// r00为最大值
		v.x = 0.5f * std::sqrt(rot.m00() - rot.m11() - rot.m22() + 1);
		KReal fCoeff = 0.25f / v.x;
		w = fCoeff * (rot.m21() - rot.m12());
		v.y = fCoeff * (rot.m10() + rot.m01());
		v.z = fCoeff * (rot.m20() + rot.m02());
	}
	else if (rot.m11() > rot.m22()) { // m00 <= m11, r11为最大值
		v.y = 0.5f * std::sqrt(rot.m11() - rot.m22() - rot.m00() + 1);
		KReal fCoeff = 0.25f / v.y;
		w = fCoeff * (rot.m02() - rot.m20());
		v.x = fCoeff * (rot.m10() + rot.m01());
		v.z = fCoeff * (rot.m21() + rot.m12());
	}
	else {// r22为最大值
		v.z = 0.5f * std::sqrt(rot.m22() - rot.m00() - rot.m11() + 1);
		KReal fCoeff = 0.25f / v.z;
		w = fCoeff * (rot.m10() - rot.m01());
		v.x = fCoeff * (rot.m20() + rot.m02());
		v.y = fCoeff * (rot.m21() + rot.m12());
	}
}

// 四元数的乘法是不可交换的
template<class KReal>
KtQuaternion<KReal> KtQuaternion<KReal>::operator*(const KtQuaternion<KReal>& rhs) const
{
	return {
		w() * rhs.x() + x() * rhs.w() + y() * rhs.z() - z() * rhs.y(),
		w() * rhs.y() - x() * rhs.z() + y() * rhs.w() + z() * rhs.x(),
		w() * rhs.z() + x() * rhs.y() - y() * rhs.x() + z() * rhs.w(),
		w() * rhs.w() - x() * rhs.x() - y() * rhs.y() - z() * rhs.z())
	};
}

// 变换局部坐标到世界坐标
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

// 四元数表示的局部坐标系各轴在世界坐标系的投影
template<class KReal>
void KtQuaternion<KReal>::ToAxes(KtVector3<KReal>& x, KtVector3<KReal>& y, KtVector3<KReal>& z) const
{
	KtMatrix3<KReal> kRot;
	ToRotationMatrix(kRot);

	// 提取旋转矩阵的各列

	x.x = kRot.m00();
	x.y = kRot.m10();
	x.z = kRot.m20();

	y.x = kRot.m01();
	y.y = kRot.m11();
	y.z = kRot.m21();

	z.x = kRot.m02();
	z.y = kRot.m12();
	z.z = kRot.m22();
}

template<class KReal>
KtVector3<KReal> KtQuaternion<KReal>::GetXAxis() const // 单独提取局部坐标x方向的单位矢量在世界坐标的投影
{
	// 1-2yy-2zz, 2xy+2wz, 2xz-2wy
//	ASSERT(IsUnit()); 
//	return KtVector3<KReal>(1-2*v.y*v.y-2*v.z*v.z, 2*v.x*v.y+2*w*v.z, 2*v.x*v.z-2*w*v.y);

	KReal dy = 2 * v.y;
	KReal dz = 2 * v.z;

	return KtVector3<KReal>(1 - dy * v.y - dz * v.z, dy * v.x + dz * w, dz * v.x - dy * w);
}

template<class KReal>
KtVector3<KReal> KtQuaternion<KReal>::GetYAxis() const // 单独提取局部坐标y方向的单位矢量在世界坐标的投影
{
	// 2xy-2wz, 1-2xx-2zz, 2yz+2wx
//	ASSERT(IsUnit()); 

	KReal dx = 2 * v.x;
	KReal dz = 2 * v.z;

	return KtVector3<KReal>(dx * v.y - dz * w, 1 - dx * v.x - dz * v.z, dz * v.y + dx * w);
}

template<class KReal>
KtVector3<KReal> KtQuaternion<KReal>::GetZAxis() const // 单独提取局部坐标z方向的单位矢量在世界坐标的投影
{
	// 2xz+2wy, 2yz-2wx, 1-2xx-2yy
//	ASSERT(IsUnit()); 

	KReal dx = 2 * v.x;
	KReal dy = 2 * v.y;

	return KtVector3<KReal>(dx * v.z + dy * w, dy * v.z - dx * w, 1 - dx * v.x - dy * v.y);
}

// 从局部坐标系相对世界坐标系的坐标来构造四元数
template<class KReal>
void KtQuaternion<KReal>::FormAxes(const KtVector3<KReal>& x, const KtVector3<KReal>& y, const KtVector3<KReal>& z)
{
	KtMatrix3<KReal> kRot;

	kRot.m00() = x.y;
	kRot.m10() = x.y;
	kRot.m20() = x.z;

	kRot.m01() = y.x;
	kRot.m11() = y.y;
	kRot.m21() = y.z;

	kRot.m02() = z.x;
	kRot.m12() = z.y;
	kRot.m22() = z.z;

	FromRotationMatrix(kRot);
}

// 滚动：绕X轴旋转
template<class KReal>
KReal KtQuaternion<KReal>::GetRoll()
{
	// roll = atan2(localy.z, localy.y);
	// 以下代码参考GetYAxis
	KReal dx = 2 * v.x;
	KReal dz = 2 * v.z;

	//	return KtVector3<KReal>(dx*v.y-dz*w, 1-dx*v.x-dz*v.z, dz*v.y+dx*w); 
	return KtuMath<KReal>::ATan2(dz * v.y + dx * w, 1 - dx * v.x - dz * v.z);
}

// 偏航：绕Z轴转动
template<class KReal>
KReal KtQuaternion<KReal>::GetYaw()
{
	// yaw = atan2(localx.y, localx.x);
	// 以下代码参考GetXAxis
	KReal dy = 2 * v.y;
	KReal dz = 2 * v.z;

	//	return KtVector3<KReal>(1-dy*v.y-dz*v.z, dy*v.x+dz*w, dz*v.x-dy*w); 
	return KtuMath<KReal>::ATan2(dy * v.x + dz * w, 1 - dy * v.y - dz * v.z);
}

// 俯仰：绕Y轴转动
template<class KReal>
KReal KtQuaternion<KReal>::GetPitch()
{
	// pitch= atan2(localz.x, localz.z);
	// 以下代码参考GetZAxis
	KReal dx = 2 * v.x;
	KReal dy = 2 * v.y;

	//	return KtVector3<KReal>(dx*v.z+dy*w, dy*v.z-dx*w, 1-dx*v.x-dy*v.y); 
	return KtuMath<KReal>::ATan2(dx * v.z + dy * w, 1 - dx * v.x - dy * v.y);
}

template<class KReal>
KtQuaternion<KReal> GetRotationBetween(const KtVector3<KReal>& kvFrom, const KtVector3<KReal>& kvTo,
	const KtVector3<KReal>& kvFallbackAxis)
{
	// Based on Stan Melax's article in Game Programming Gems
	KtQuaternion<KReal> q;
	// Copy, since cannot modify local
	KtVector3<KReal> v0 = kvFrom.GetNormalize();
	KtVector3<KReal> v1 = kvTo.GetNormalize();

	KReal d = v0.Dot(v1);
	// If dot == 1, vectors are the same
	if (d >= 1.0f)
	{
		return KtQuaternion<KReal>::IDENTITY;
	}
	if (d < (1e-6f - 1.0f))
	{
		if (kvFallbackAxis != KtVector3<KReal>::ZERO)
		{
			// rotate 180 degrees about the fallback axis
			q.FromAngleAxis(KtuMath<KReal>::PI, kvFallbackAxis);
		}
		else
		{
			// Generate an axis
			KtVector3<KReal> kvAxis = kvFrom.Perpendicular();
			q.FromAngleAxis(KtuMath<KReal>::PI, kvAxis);
		}
	}
	else
	{
		KReal s = std::sqrt((1 + d) * 2);
		KReal invs = 1 / s;

		KtVector3<KReal> c = v0.Cross(v1);

		q.v.x = c.x * invs;
		q.v.y = c.y * invs;
		q.v.z = c.z * invs;
		q.w = s * 0.5f;
		q.Normalize();
	}
	return q;
}

using quatf = KtQuaternion<float>;
using quatd = KtQuaternion<double>;

