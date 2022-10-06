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

	// 重载零构造
	KtQuaternion() : super_(0, 0, 0, 1) {}

	 /// Construct a quaternion from a rotation matrix
	template<bool ROW_MAJOR>
	KtQuaternion(const mat3<ROW_MAJOR>& rot);

	/// Construct a quaternion from an angle/axis
	KtQuaternion(KReal angle, const vec3& axis);

	// 从局部坐标系相对世界坐标系的坐标来构造四元数
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

	// 四元数的乘法是不可交换的
	KtQuaternion operator*(const KtQuaternion& rhs) const;

	// 将原坐标系中的矢量v变换到目标坐标系
	vec3 operator*(const vec3& v) const; 
 
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

	template<bool ROW_MAJOR>
	void toRotateMatrix(mat3<ROW_MAJOR>& rot) const;

	void toAxes(vec3& x, vec3& y, vec3& z) const;

	vec3 xAxis() const; // 单独提取局部坐标x方向的单位矢量在世界坐标的投影
	vec3 yAxis() const; // 单独提取局部坐标y方向的单位矢量在世界坐标的投影
	vec3 zAxis() const; // 单独提取局部坐标z方向的单位矢量在世界坐标的投影

	// 获取当前局部坐标系的俯仰（绕右向轴Z旋转）、偏航（绕上轴Y旋转）和滚动（绕方向轴X旋转）角
	KReal pitch(); // 俯仰
	KReal yaw(); // 偏航
	KReal roll(); // 滚动

};


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
	else if (rot.m00() > rot.m11() && rot.m00() > rot.m22()) {// r00为最大值
		x() = 0.5f * std::sqrt(rot.m00() - rot.m11() - rot.m22() + 1);
		KReal f = 0.25f / x();
		w() = f * (rot.m21() - rot.m12());
		y() = f * (rot.m10() + rot.m01());
		z() = f * (rot.m20() + rot.m02());
	}
	else if (rot.m11() > rot.m22()) { // m00 <= m11, r11为最大值
		y() = 0.5f * std::sqrt(rot.m11() - rot.m22() - rot.m00() + 1);
		KReal f = 0.25f / y();
		w() = f * (rot.m02() - rot.m20());
		x() = f * (rot.m10() + rot.m01());
		z() = f * (rot.m21() + rot.m12());
	}
	else {// r22为最大值
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
		// 绕任意轴旋转180度，此处选择from的垂直轴
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

//     | ww+xx-yy-zz    2xy-2wz       2xz+2wy   |
// R = |   2xy+2wz    ww+yy-xx-zz     2yz-2wx   |
//     |   2xz-2wy       2yz+2wx    ww+zz-xx-yy |
// 对于单位四元数，可以简化为：
//     | 1-2yy-2zz    2xy-2wz     2xz+2wy  |
// R = |  2xy+2wz    1-2xx-2zz    2yz-2wx  |
//     |  2xz-2wy     2yz+2wx    1-2xx-2yy |
template<class KReal> template<bool ROW_MAJOR>
void KtQuaternion<KReal>::toRotateMatrix(mat3<ROW_MAJOR>& rot) const
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

template<class KReal>
void KtQuaternion<KReal>::toAxes(vec3& x, vec3& y, vec3& z) const
{
	mat3<> rot;
	toRotateMatrix(rot);

	// 提取旋转矩阵的各列
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

// 滚动：绕X轴旋转
template<class KReal>
KReal KtQuaternion<KReal>::roll()
{
	// roll = atan2(localy.z, localy.y);

	KReal dx = 2 * v.x;
	KReal dz = 2 * v.z;

	return std::atan2(dz * y() + dx * w(), 1 - dx * x() - dz * z());
}

// 偏航：绕Z轴转动
template<class KReal>
KReal KtQuaternion<KReal>::yaw()
{
	// yaw = atan2(localx.y, localx.x);

	KReal dy = 2 * v.y;
	KReal dz = 2 * v.z;
 
	return std::atan2(dy * x() + dz * w(), 1 - dy * y() - dz * z());
}

// 俯仰：绕Y轴转动
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

