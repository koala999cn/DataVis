#pragma once
#include "KtuMath.h"

template<class KReal>
class KtVector3
{
	using kMath = KtuMath<KReal>;

public:
	/// ����
	KtVector3() { }
	KtVector3(KReal _x, KReal _y, KReal _z) : x(_x), y(_y), z(_z) { }
	KtVector3(const KtVector3& v) : x(v.x), y(v.y), z(v.z) { }
	KtVector3(const KReal data[]) : x(data[0]), y(data[1]), z(data[2]) { }

	/// ����
	KtVector3& operator=(const KtVector3& v) { 
		x = v.x, y = v.y, z = v.z;
		return *this;
	}

	KtVector3& operator=(KReal fScalar) { 
		x = y = z = fScalar;
		return *this;
	}


	/// �Ƚ�
	bool operator==(const KtVector3& v) const { 
		return x == v.x && y == v.y && z == v.z; 
	}

	bool operator!=(const KtVector3& v) const { 
		return x != v.x || y != v.y || z != v.z; 
	}

	bool isZero() const { 
		return x == 0 && y == 0 && z == 0; 
	}

	bool isApproxEqual(const KtVector3& v) const { 
		return kMath::approxEqual(x, v.x) 
			&& kMath::approxEqual(y, v.y) 
			&& kMath::approxEqual(z, v.z); 
	}

	bool isApproxZero() const { 
		return isApproxEqual(KtVector3(0, 0, 0));
	}

	/// һ������

	KtVector3& operator+() { return *this; }
	KtVector3 operator-() const { return KtVector3(-x, -y, -z); }

	KtVector3 operator+(const KtVector3& rhs) const { 
		return KtVector3(x + rhs.x, y + rhs.y, z + rhs.z);
	}
	KtVector3 operator-(const KtVector3& rhs) const { 
		return KtVector3(x - rhs.x, y - rhs.y, z - rhs.z);
	}
	KtVector3 operator*(const KtVector3& rhs) const { 
		return KtVector3(x * rhs.x, y * rhs.y, z * rhs.z);
	}
	KtVector3 operator/(const KtVector3& rhs) const { 
		return KtVector3(x / rhs.x, y / rhs.y, z / rhs.z);
	}

	KtVector3 operator+(KReal fScalar) const { 
		return KtVector3(x + fScalar, y + fScalar, z + fScalar);
	}
	KtVector3 operator-(KReal fScalar) const { 
		return KtVector3(x - fScalar, y - fScalar, z - fScalar);
	}
	KtVector3 operator*(KReal fScalar) const { 
		return KtVector3(x * fScalar, y * fScalar, z * fScalar);
	}
	KtVector3 operator/(KReal fScalar) const { 
		KReal fFactor = 1.0f / fScalar;
		return KtVector3(x * fFactor, y * fFactor, z * fFactor);
	}

	KtVector3& operator+=(const KtVector3& v) { 
		x += v.x, y += v.y, z += v.z;
		return *this; 
	}
	KtVector3& operator-=(const KtVector3& v) { 
		x -= v.x, y -= v.y, z -= v.z;
		return *this; 
	}
	KtVector3& operator*=(const KtVector3& v) { 
		x *= v.x, y *= v.y, z *= v.z;
		return *this; 
	}
	KtVector3& operator/=(const KtVector3& v) { 
		x /= v.x, y /= v.y, z /= v.z;
		return *this; 
	}

	KtVector3& operator+=(KReal fScalar) { 
		x += fScalar, y += fScalar, z += fScalar;
		return *this; 
	}
	KtVector3& operator-=(KReal fScalar) { 
		x -= fScalar, y -= fScalar, z -= fScalar;
		return *this; 
	}
	KtVector3& operator*=(KReal fScalar) { 
		x *= fScalar, y *= fScalar, z *= fScalar;
		return *this; 
	}
	KtVector3& operator/=(KReal fScalar) { 
		KReal fFactor = 1.0f / fScalar; x *= fFactor, y *= fFactor, z *= fFactor;
		return *this; 
	}


	/// ������

	KReal distance(const KtVector3& v) const { 
		return std::sqrt(squaredDistance(v));
	} 

	KReal squaredDistance(const KtVector3& v) const { 
		return (x - v.x) * (x - v.x) + (y - v.y) * (y - v.y) + (z - v.z) * (z - v.z);
	}

	KReal invDistance(const KtVector3& v) const { 
		return kMath::invSqrt(squaredDistance(v));
	}

	/// ʸ������

	KReal length() const { return std::sqrt(x * x + y * y + z * z); } // ʸ������
	KReal squaredLength() const { return x * x + y * y + z * z; }
	KReal invLength() const { return kMath::invSqrt(x * x + y * y + z * z); } // ���ȵĵ���

	// ʹ�ÿ����㷨���㳤�ȵĽ���ֵ-ժ�ԡ�Tricks of the 3D game programming gurus���ĵ�9��
	KReal approxLength() const {
		// ȷ�����е�ֵ��Ϊ��
		int ix = (int)(std::abs(x) * 1024);
		int iy = (int)(std::abs(y) * 1024);
		int iz = (int)(std::abs(z) * 1024);

		// ����
		if (iy < ix) std::swap(ix, iy);
		if (iz < iy) std::swap(iy, iz);
		if (iy < ix) std::swap(ix, iy);

		int d = iz + 11 * (iy >> 5) + (ix >> 2);

		// ����ľ������С��8%
		return (KReal)(d >> 10);
	}

	const KtVector3& normalize() { // �Ե�ǰʸ�����й淶������
		KReal fLen2 = squaredLength();
		if (fLen2 > 1e-8) // ���ʸ������̫С���򲻽��й淶������
			*this *= 1 / std::sqrt(fLen2);

		return *this;
	}

	// ���
	KReal dot(const KtVector3& rhs) const { 
		return x * rhs.x + y * rhs.y + z * rhs.z; 
	} 

	KReal absDot(const KtVector3& rhs) const {
		return std::abs(x * rhs.x) + std::abs(y * rhs.y) + std::abs(z * rhs.z);
	}

	// ���
	KtVector3 cross(const KtVector3& rhs) const { 
		return KtVector3(y * rhs.z - rhs.y * z, z * rhs.x - rhs.z * x, x * rhs.y - rhs.x * y);
	} 

	// ��ʸ��v֮��ļнǣ����ȣ�
	KReal angle(const KtVector3& v) const {
		KReal f = invLength() * v.invLength() * dot(v);
		f = kMath::clamp((KReal)-1.0f, (KReal)1.0f);
		return std::acos(f); // radian
	}

	// ����this��ʸ��v�ϵ�ͶӰ
	KtVector3 projectedTo(const KtVector3& v) const { 
		return v * dot(v) / v.squaredLength(); 
	} 

	// ƽ�в���
	bool isParallelTo(const KtVector3& v) const { 
		return Cross(v).isApproxZero(); 
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

	static KtVector3 min(const KtVector3& v1, const KtVector3& v2)
	{
		return KtVector3(std::min(v1.x, v2.x),
			std::min(v1.y, v2.y),
			std::min(v1.z, v2.z));
	}

	static KtVector3 max(const KtVector3& v1, const KtVector3& v2)
	{
		return KtVector3(std::max(v1.x, v2.x),
			std::max(v1.y, v2.y),
			std::max(v1.z, v2.z));
	}


	///constexpr static KtVector3 zero;
	//constexpr static KtVector3 unit_x;
	//constexpr static KtVector3 unit_y;
	//constexpr static KtVector3 unit_z;

	// ָ����Ļ�ڵ�����
	template<bool LEFT_HAND = true>
	static constexpr KtVector3 forward() {
		if constexpr (LEFT_HAND)
			return KtVector3(0, 0, 1); // ��������ϵ
		else
			return KtVector3(0, 0, -1); // ��������ϵ
	}

public:
	// ���ݳ�Ա
	KReal x, y, z;
};

/*
template<class KReal>
constexpr KtVector3<KReal> KtVector3<KReal>::zero = KtVector3<KReal>(0, 0, 0);

template<class KReal>
constexpr KtVector3<KReal> KtVector3<KReal>::unit_x = KtVector3<KReal>(1, 0, 0);

template<class KReal>
constexpr KtVector3<KReal> KtVector3<KReal>::unit_y = KtVector3<KReal>(0, 1, 0);

template<class KReal>
constexpr KtVector3<KReal> KtVector3<KReal>::unit_z = KtVector3<KReal>(0, 0, 1);
*/

using float3 = KtVector3<float> ;
using double3 = KtVector3<double>;

