#pragma once
#include "KtPoint.h"
#include "KtVector3.h"


template<class KReal>
class KtVector4 : public KtPoint<KReal, 4>
{
	using super_ = KtPoint<KReal, 4>;

public:
	using super_::x;
	using super_::y;
	using super_::z;
	using super_::w;

	using super_::super_;

	KtVector4(const super_& pt) : super_(pt) {}

	// 若v为点，保留w=1; 若为矢量，置w=0
	explicit KtVector4(const KtPoint<KReal, 3>& v, KReal _w = 1) {
		x() = v.x(), y() = v.y(), z() = v.z();
		w() = _w;
	}

	// 在齐次坐标下，矢量的w为0，点的w非零.
	// 这样在乘以齐次变换矩阵的时候，矩阵中的位移部分不会影响矢量
	bool isPoint() const { return w() != 0; }
	bool isVector() const { return w() = 0; }

	KtVector4& homogenize() {
		if(w() != 0 && w() != 1) { 
			KReal f = 1 / w();
			x() *= f, y() *= f, z() *= f, w() = 1;
		} 

		return *this;
	}

	KtVector3<KReal> vec3() const { return { x(), y(), z() }; }
};

using vec4f = KtVector4<float> ;
using vec4d = KtVector4<double>;

using float4 = vec4f;
using double4 = vec4d;

