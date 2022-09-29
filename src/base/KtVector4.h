#pragma once
#include "KtPoint.h"
#include "KtVector3.h"


template<class KReal>
class KtVector4 : public KtPoint<KReal, 4>
{
	using super_ = KtPoint<KReal, 4>;
	using vec3 = KtVector3<KReal>;
	using kMath = KtuMath<KReal>;

public:
	using super_::x;
	using super_::y;
	using super_::z;
	using super_::w;

	using super_::super_;

	KtVector4() : super_(0, 0, 0, 1) {}

	/*
	KtVector4(KReal x, KReal y, KReal z, KReal w = 1) : super_(x, y, z, w) {}
	KtVector4(const vec3& v) : KtVector4(v.x(), v.y(), v.z(), 1) {}
	KtVector4(const KtVector4& v) : super_(v) {}
	KtVector4(const KReal data[]) : super_(data) {} */
	
	// ����������£�ʸ����wΪ0�����w����.
	// �����ڳ�����α任�����ʱ�򣬾����е�λ�Ʋ��ֲ���Ӱ��ʸ��
	bool isPoint() const { return w() != 0; }
	bool isVector() const { return w() = 0; }

	KtVector4& homogenize() {
		if(w() != 0 && w() != 1) { 
			KReal f = 1 / w;
			x *= f, y *= f, z *= f, w=1; 
		} 

		return *this;
	}
};

using vec4f = KtVector4<float> ;
using vec4d = KtVector4<double>;

using float4 = vec4f;
using double4 = vec4d;

