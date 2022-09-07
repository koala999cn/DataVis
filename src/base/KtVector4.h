#pragma once
#include "KtVector3.h"


template<class KReal>
class KtVector4 : public KtVector3<KReal>
{
	using super_ = KtVector3<KReal>;
	using super_::kMath;

public:
	KtVector4() : w(1) { }

	KtVector4(KReal _x, KReal _y, KReal _z, KReal _w = 1) : super_(_x, _y, _z), w(_w) { }
	KtVector4(const super_& v) : super_(v), w(1) { }
	KtVector4(const KtVector4& v) : super_(v.x, v.y, v.z), w(v.w) { }
	KtVector4(const KReal data[]) : super_(data), w(data[3]) { }
	
	// 在齐次坐标下，矢量的w为0，点的w非零.
	// 这样在乘以齐次变换矩阵的时候，矩阵中的位移部分不会影响矢量
	bool isPoint() const { return w != 0; }
	bool isVector() const { return w = 0; }

	const KtVector4& homogenize() {
		if(w!=0 && w!=1) { 
			KReal f = 1/w; 
			x*=f, y*=f, z*=f, w=1; 
		} 

		return *this;
	}

	// 复制
	KtVector4& operator=(const super_& v) {
		x = v.x, y = v.y, z = v.z, w = 1; 
		return *this; 
	}
	KtVector4& operator=(const KtVector4& v) { 
		x = v.x, y = v.y, z = v.z, w = v.w; 
		return *this; 
	}
	KtVector4& operator=(KReal fScalar) { 
		x = y = z = w = fScalar; 
		return *this; 
	}
	
	// 比较
	bool operator==(const KtVector4& v) const { 
		return super_::operator ==(v) && w == v.w;
	}
	bool operator!=(const KtVector4& v) const { 
		return super_::operator !=(v) || w != v.w; 
	}

	bool isZero() const { 
		return super_::isZero() && w == 0; 
	}
	bool isApproxEqual(const KtVector4& v) const { 
		return super_::isApproxEqual(v) && kMath::approxEqual(w, v.w);
	}
	bool isApproxZero() const { 
		return super_::isApproxZero() && kMath::approxEqual(w, 0); 
	}
	
	// 一般运算
	KtVector4& operator+() { return *this; }
	KtVector4 operator-() const { return KtVector4(-x, -y, -z, -w); }
	
	KtVector4 operator+(const KtVector4& rhs) const { 
		return KtVector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
	}
	KtVector4 operator-(const KtVector4& rhs) const { 
		return KtVector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
	}
	KtVector4 operator*(const KtVector4& rhs) const {
		return KtVector4(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w);
	}
	KtVector4 operator/(const KtVector4& rhs) const {
		return KtVector4(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w);
	}
	
	KtVector4 operator+(KReal fScalar) const { 
		return KtVector4(x + fScalar, y + fScalar, z + fScalar, w + fScalar);
	}
	KtVector4 operator-(KReal fScalar) const { 
		return KtVector4(x - fScalar, y - fScalar, z - fScalar, w - fScalar);
	}
	KtVector4 operator*(KReal fScalar) const { 
		return KtVector4(x * fScalar, y * fScalar, z * fScalar, w * fScalar);
	}
	KtVector4 operator/(KReal fScalar) const { 
		KReal fFactor = 1.0f / fScalar;
		return KtVector4(x * fFactor, y * fFactor, z * fFactor, w * fFactor);
	}
	
	KtVector4& operator+=(const KtVector4& v) { 
		x += v.x, y += v.y, z += v.z, w += v.w;
		return *this; 
	}
	KtVector4& operator-=(const KtVector4& v) { 
		x -= v.x, y -= v.y, z -= v.z, w -= v.w;
		return *this; 
	}
	KtVector4& operator*=(const KtVector4& v) {
		x *= v.x, y *= v.y, z *= v.z, w *= v.w;
	    return *this; 
    }
	KtVector4& operator/=(const KtVector4& v) { 
		x /= v.x, y /= v.y, z /= v.z, w /= v.w;
		return *this; 
	}
	
	KtVector4& operator+=(KReal fScalar) { 
		x += fScalar, y += fScalar, z += fScalar, w += fScalar;
		return *this; 
	}
	KtVector4& operator-=(KReal fScalar) { 
		x -= fScalar, y -= fScalar, z -= fScalar, w -= fScalar;
		return *this; 
	}
	KtVector4& operator*=(KReal fScalar) { 
		x *= fScalar, y *= fScalar, z *= fScalar, w *= fScalar;
		return *this; 
	}
	KtVector4& operator/=(KReal fScalar) { 
		KReal fFactor = 1.0f / fScalar;
		x *= fFactor, y *= fFactor, z *= fFactor, w *= fFactor;
		return *this;
	}

public:
	KReal w;
};

using float4 = KtVector4<float> ;
using double4 = KtVector4<double>;


