#pragma once
#include "KtVector3.h"
#include "KtVector4.h"
#include "KtMatrix4.h"
#include "KtQuaternion.h"

template<typename REAL, bool ROW_MAJOR = true>
class KtCamera
{
	using point2 = KtPoint<REAL, 2>;
	using vec3 = KtVector3<REAL>;
	using vec4 = KtVector4<REAL>;
	using mat3 = KtMatrix3<REAL, ROW_MAJOR>;
	using mat4 = KtMatrix4<REAL, ROW_MAJOR>;
	using quat = KtQuaternion<REAL>;

public:

	KtCamera();

	// �����λ��eye�㣬�Ϸ�����up������at�㣬���˸���viewMatrix
	void lookAt(const vec3& eye, const vec3& at, const vec3& up);

	// ��frustum��������projMatrix_
	void projectFrustum(REAL left, REAL right, REAL bottom, REAL top, REAL znear, REAL zfar);

	void projectPerspective(REAL fovyInDegree = 45, REAL aspectRatio = 4.f/3.f, REAL znear = 100, REAL zfar = 10000);

	void projectOrtho(REAL left, REAL right, REAL bottom, REAL top, REAL znear, REAL zfar);

	const mat4& viewMatrix() const { return viewMatrix_; }
	mat4& viewMatrix() { return viewMatrix_; }

	const mat4& projMatrix() const { return projMatrix_; }
	mat4& projMatrix() { return projMatrix_; }

	void setViewport(REAL x, REAL y, REAL width, REAL height) {
		x_ = x, y_ = y, w_ = width, h_ = height;
	}

	vec3 getCameraPos() const;
	quat getCameraOrient() const;

	// ��world�����/ʸ��in����ͶӰ����ת����viewport����ϵ
	bool project(const vec4& in, vec4& out) const;

	// ��viewport�����in���з�ͶӰ����ת����world����ϵ
	bool unproject(const vec3& in, vec4& out) const;

	// ����ray������
	// ray��ԭ���ͨ��getCameraPos��ȡ
	vec3 screenPointToRay(const point2& pt) const;
	
	point2 worldToViewport(const vec4& in) const {
		vec4 out;
		project(in, out);
		return { out.x(), out.y() };
	}

	point2 worldToScreen(const vec4& in) {
		return switchViewportAndScreen(worldToViewport(in));
	}

	// viewport���½�Ϊ(0, 0)�㣬screen���Ͻ�Ϊ(0, 0)��
	point2 switchViewportAndScreen(const point2& pt) const {
		return { pt.x, h_ - pt.y };
	}


protected:

	// Viewing window. 
	REAL x_{ 0 }, y_{ 0 }, w_{ 1 }, h_{ 1 };

	// view matrix
	mat4 viewMatrix_; // viewMatrix_ * ��������ϵvec4 = ���������ϵvec4

	// standard projection matrix
	mat4 projMatrix_;
};


template<typename REAL, bool ROW_MAJOR>
KtCamera<REAL, ROW_MAJOR>::KtCamera()
{
	viewMatrix_ = projMatrix_ = mat4::identity();
}


template<typename REAL, bool ROW_MAJOR>
void KtCamera<REAL, ROW_MAJOR>::lookAt(const vec3& eye, const vec3& at, const vec3& up)
{
	vec3 zaxis = (eye - at).normalize(); // ���������ϵ����������ϵ��z�����෴�ģ����Դ˴�ȡ����
	vec3 xaxis = up.cross(zaxis).normalize();
	vec3 yaxis = zaxis.cross(xaxis);

	// look at view

	// ���1��Ϊeye��������������ͶӰȡ��
	// ���������������ϵ��(a, b, c)�㣬�൱����������ϵԭ�������������ϵ��(-a, -b, -c)��
	viewMatrix_ = {
	   xaxis.x(), xaxis.y(), xaxis.z(), -xaxis.dot(eye),
	   yaxis.x(), yaxis.y(), yaxis.z(), -yaxis.dot(eye),
	   zaxis.x(), zaxis.y(), zaxis.z(), -zaxis.dot(eye),
			   0,         0,         0,               1
	};
}


template<typename REAL, bool ROW_MAJOR>
void KtCamera<REAL, ROW_MAJOR>::projectFrustum(REAL left, REAL right, REAL bottom, REAL top, REAL znear, REAL zfar)
{
	// NB: This creates 'uniform' perspective projection matrix,
	// which depth range [-1,1]
	// �����������λ����㣬��znearӳ�䵽-1, zfarӳ�䵽+1
	// 
	// right-handed rules
	//
	// [ x   0   a   0  ]
	// [ 0   y   b   0  ]
	// [ 0   0   c   d ]
	// [ 0   0   -1  0  ]
	//
	// left-handed rules
	//
	// [ x   0   a   0  ]
	// [ 0   y   b   0  ]
	// [ 0   0   -c  d  ]
	// [ 0   0   1   0  ]
	//
	// x = 2 * near / (right - left)
	// y = 2 * near / (top - bottom)
	// a = (right + left) / (right - left)
	// b = (top + bottom) / (top - bottom)
	// c = - (far + near) / (far - near)
	// d = - 2 * (far * near) / (far - near)

	if (znear <= 0 || zfar <= 0 || znear == zfar || left == right || top == bottom)
		projMatrix_ = mat4::zero();

	auto x = (2 * znear) / (right - left);
	auto y = (2 * znear) / (top - bottom);
	auto a = (right + left) / (right - left);
	auto b = (top + bottom) / (top - bottom);
	auto c = -(zfar + znear) / (zfar - znear);
	auto d = -(2 * zfar * znear) / (zfar - znear);

	projMatrix_ = {
		x,    0,    a,    0,
		0,    y,    b,    0,
		0,    0,    c,    d,
		0,    0,   -1,    0
	};
}


template<typename REAL, bool ROW_MAJOR>
void KtCamera<REAL, ROW_MAJOR>::projectPerspective(REAL fovyInDegree, REAL aspectRatio, REAL znear, REAL zfar)
{
	REAL ymax, xmax;
	ymax = znear * std::tan(fovyInDegree * KtuMath<REAL>::pi / 180);
	xmax = ymax * aspectRatio;
	projectFrustum(-xmax, xmax, -ymax, ymax, znear, zfar);
}


template<typename REAL, bool ROW_MAJOR>
void KtCamera<REAL, ROW_MAJOR>::projectOrtho(REAL left, REAL right, REAL bottom, REAL top, REAL znear, REAL zfar)
{
	// NB: This creates 'uniform' orthographic projection matrix,
	// which depth range [-1, +1], right-handed rules
	//
	// [ A   0   0   C  ]
	// [ 0   B   0   D  ]
	// [ 0   0   q   qn ]
	// [ 0   0   0   1  ]
	//
	// A = 2 * / (right - left)
	// B = 2 * / (top - bottom)
	// C = - (right + left) / (right - left)
	// D = - (top + bottom) / (top - bottom)
	// q = - 2 / (far - near)
	// qn = - (far + near) / (far - near)

	auto A = 2 * / (right - left);
	auto B = 2 * / (top - bottom);
	auto C = -(right + left) / (right - left);
	auto D = -(top + bottom) / (top - bottom);
	auto q = -2 / (zfar - znear);
	auto qn = -(zfar + znear) / (zfar - znear);

	projMatrix_ = {
		A, 0, 0, C,
		0, B, 0, D,
		0, 0, q, qn,
		0, 0, 0, 1
	};
}


template<typename REAL, bool ROW_MAJOR>
bool KtCamera<REAL, ROW_MAJOR>::project(const vec4& in, vec4& out) const
{
	out = projMatrix_ * viewMatrix_ * in;

	if (out.w() == 0.0f)
		return false;

	// ��ʱoutΪ���᷶ΧΪ[-1, +1]�Ĺ淶������
	out.homogenize();

	// map to range [0, 1]
	out.x() = out.x() * 0.5f + 0.5f;
	out.y() = out.y() * 0.5f + 0.5f;
	out.z() = out.z() * 0.5f + 0.5f;

	// map to viewport
	out.x() = out.x() * w_ + x_;
	out.y() = out.y() * h_ + y_;
	return true;
}


template<typename REAL, bool ROW_MAJOR>
bool KtCamera<REAL, ROW_MAJOR>::unproject(const vec3& in, vec4& out) const
{
	vec4 v(in);

	// map from viewport to [0, 1]
	v.x() = (v.x() - x_) / w_;
	v.y() = (v.y() - y_) / h_;

	// map to range [-1, +1]
	v.x() = v.x() * 2.0f - 1.0f;
	v.y() = v.y() * 2.0f - 1.0f;
	v.z() = v.z() * 2.0f - 1.0f;

	mat4 inverse = (projMatrix_ * viewMatrix_).getInversed();

	v = inverse * v;
	if (v.w() == 0.0)
		return false;

	out = v / v.w();
	return true;
}


template<typename REAL, bool ROW_MAJOR>
KtVector3<REAL> KtCamera<REAL, ROW_MAJOR>::screenPointToRay(const point2& pt) const
{
	auto vpt = switchViewportAndScreen(pt);
	vec4 out;
	unproject({ vpt.x(), vpt.y(), 0 }, out);
	return (vec3(out.x(), out.y(), out.z()) - getCameraPos()).normalize();
}


template<typename REAL, bool ROW_MAJOR>
KtVector3<REAL> KtCamera<REAL, ROW_MAJOR>::getCameraPos() const
{
	return viewMatrix_.getTranslation();
}


template<typename REAL, bool ROW_MAJOR>
KtQuaternion<REAL> KtCamera<REAL, ROW_MAJOR>::getCameraOrient() const
{
	return viewMatrix_.getRotation(); // TODO:
}
