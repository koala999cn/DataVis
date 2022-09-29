#pragma once
#include "KtPoint.h"


template<class KReal>
class KtVector3 : public KtPoint<KReal, 3>
{
	using super_ = KtPoint<KReal, 3>;
	using kMath = KtuMath<KReal>;

public:
	/// ����
	using super_::super_;

	using super_::x;
	using super_::y;
	using super_::z;

	/// ʸ������

	KReal length() const { return super_::abs(); } // ʸ������
	KReal squaredLength() const { return x() * x() + y() * y() + z() * z(); }

	// ʹ�ÿ����㷨���㳤�ȵĽ���ֵ-ժ�ԡ�Tricks of the 3D game programming gurus���ĵ�9��
	KReal approxLength() const {
		// ȷ�����е�ֵ��Ϊ��
		int ix = (int)(std::abs(x()) * 1024);
		int iy = (int)(std::abs(y()) * 1024);
		int iz = (int)(std::abs(z()) * 1024);

		// ����
		if (iy < ix) std::swap(ix, iy);
		if (iz < iy) std::swap(iy, iz);
		if (iy < ix) std::swap(ix, iy);

		int d = iz + 11 * (iy >> 5) + (ix >> 2);

		// ����ľ������С��8%
		return (KReal)(d >> 10);
	}

	// ���
	KReal dot(const KtVector3& rhs) const { 
		return x() * rhs.x() + y() * rhs.y() + z() * rhs.z(); 
	} 

	KReal absDot(const KtVector3& rhs) const {
		return std::abs(x() * rhs.x()) + std::abs(y() * rhs.y()) + std::abs(z() * rhs.z());
	}

	// ���
	KtVector3 cross(const KtVector3& rhs) const { 
		return KtVector3(y() * rhs.z() - rhs.y() * z(), 
			z() * rhs.x() - rhs.z() * x(), 
			x() * rhs.y() - rhs.x() * y());
	} 

	// ��ʸ��v֮��ļнǣ����ȣ�
	KReal angle(const KtVector3& v) const {
		KReal f = dot(v) / length() / v.length();
		f = kMath::clamp(f, (KReal)-1.0f, (KReal)1.0f);
		return std::acos(f); // radian
	}

	// ����this��ʸ��v�ϵ�ͶӰ
	KtVector3 projectedTo(const KtVector3& v) const { 
		return v * dot(v) / v.squaredLength(); 
	} 

	// ƽ�в���
	bool isParallelTo(const KtVector3& v) const { 
		return cross(v).isApproxZero(); 
	} 

	// ��ֱ����
	bool isPerpendicularTo(const KtVector3& v) const { 
		return kMath::approxEqual(dot(v), 0.0f); 
	} 

	// ���㵱ǰ�����Ĵ�ֱ����
	KtVector3 perpendicular() const {
		KtVector3 perp = cross(KtVector3(1, 0, 0));

		if (perp.isApproxZero())
			perp = cross(KtVector3(0, 1, 0));

		perp.normalize();
		return perp;
	}


	// ��������

	// �������㹹��ƽ��ķ��ߣ�����v1, v2, v3����ʱ������
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

	// ָ����Ļ�ڵ�����(Ĭ��x����������y����������)
	template<bool LEFT_HAND = true>
	static const KtVector3& forward() {
		if constexpr (LEFT_HAND) // ��������ϵ
			return unitZ(); 
		else { // ��������ϵ
			static KtVector3 f = -unitZ();
			return f; 
		}
	}
};

using vec3f = KtVector3<float> ;
using vec3d = KtVector3<double>;

