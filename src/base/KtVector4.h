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

	KtVector4(const super_& pt) : super_(pt) {}

	// ��vΪ�㣬����w=1; ��Ϊʸ������w=0
	KtVector4(const KtPoint<KReal, 3>& v, KReal w = 1) {
		x() = v.x(), y() = v.y(), z() = v.z();
		w() = w;
	}

	// ����������£�ʸ����wΪ0�����w����.
	// �����ڳ�����α任�����ʱ�򣬾����е�λ�Ʋ��ֲ���Ӱ��ʸ��
	bool isPoint() const { return w() != 0; }
	bool isVector() const { return w() = 0; }

	KtVector4& homogenize() {
		if(w() != 0 && w() != 1) { 
			KReal f = 1 / w();
			x() *= f, y() *= f, z() *= f, w = 1;
		} 

		return *this;
	}
};

using vec4f = KtVector4<float> ;
using vec4d = KtVector4<double>;

using float4 = vec4f;
using double4 = vec4d;

