#pragma once
#include "KtPoint.h"


template<class KReal>
class KtVector3 : public KtPoint<KReal, 3>
{
	using super_ = KtPoint<KReal, 3>;
	using kMath = KtuMath<KReal>;

public:
	/// 构造
	using super_::super_;

	using super_::x;
	using super_::y;
	using super_::z;

	/// 矢量运算

	KReal length() const { return super_::abs(); } // 矢量长度
	KReal squaredLength() const { return x() * x() + y() * y() + z() * z(); }

	// 使用快速算法计算长度的近似值-摘自《Tricks of the 3D game programming gurus》的第9章
	KReal approxLength() const {
		// 确保所有的值皆为正
		int ix = (int)(std::abs(x()) * 1024);
		int iy = (int)(std::abs(y()) * 1024);
		int iz = (int)(std::abs(z()) * 1024);

		// 排序
		if (iy < ix) std::swap(ix, iy);
		if (iz < iy) std::swap(iy, iz);
		if (iy < ix) std::swap(ix, iy);

		int d = iz + 11 * (iy >> 5) + (ix >> 2);

		// 计算的距离误差小于8%
		return (KReal)(d >> 10);
	}

	// 点积
	KReal dot(const KtVector3& rhs) const { 
		return x() * rhs.x() + y() * rhs.y() + z() * rhs.z(); 
	} 

	KReal absDot(const KtVector3& rhs) const {
		return std::abs(x() * rhs.x()) + std::abs(y() * rhs.y()) + std::abs(z() * rhs.z());
	}

	// 叉乘
	KtVector3 cross(const KtVector3& rhs) const { 
		return KtVector3(y() * rhs.z() - rhs.y() * z(), 
			z() * rhs.x() - rhs.z() * x(), 
			x() * rhs.y() - rhs.x() * y());
	} 

	// 和矢量v之间的夹角（弧度）
	KReal angle(const KtVector3& v) const {
		KReal f = dot(v) / length() / v.length();
		f = kMath::clamp(f, (KReal)-1.0f, (KReal)1.0f);
		return std::acos(f); // radian
	}

	// 计算this在矢量v上的投影
	KtVector3 projectedTo(const KtVector3& v) const { 
		return v * dot(v) / v.squaredLength(); 
	} 

	// 平行测试
	bool isParallelTo(const KtVector3& v) const { 
		return cross(v).isApproxZero(); 
	} 

	// 垂直测试
	bool isPerpendicularTo(const KtVector3& v) const { 
		return kMath::approxEqual(dot(v), 0.0f); 
	} 

	// 计算当前向量的垂直向量
	KtVector3 perpendicular() const {
		KtVector3 perp = cross(KtVector3(1, 0, 0));

		if (perp.isApproxZero())
			perp = cross(KtVector3(0, 1, 0));

		perp.normalize();
		return perp;
	}


	// 辅助函数

	// 计算三点构成平面的法线，其中v1, v2, v3按逆时针排列
	static KtVector3 normal(const KtVector3& v1, const KtVector3& v2, const KtVector3& v3) { 
		return (v2 - v1).cross(v3 - v2).normalize();
	}


	static const KtVector3& unitX() {
		static KtVector3 x(1, 0, 0);
		return x;
	}

	static const KtVector3& unitY() {
		static KtVector3 y(0, 1, 0);
		return y;
	}

	static const KtVector3& unitZ() {
		static KtVector3 z(0, 0, 1);
		return z;
	}

	// 指向屏幕内的向量(默认x轴正向向左，y轴正向向上)
	template<bool LEFT_HAND = true>
	static const KtVector3& forward() {
		if constexpr (LEFT_HAND) // 左手坐标系
			return unitZ(); 
		else { // 右手坐标系
			static KtVector3 f = -unitZ();
			return f; 
		}
	}
};

using vec3f = KtVector3<float> ;
using vec3d = KtVector3<double>;

