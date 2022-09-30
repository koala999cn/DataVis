#pragma once
#include "KtVector3.h"
#include "KtVector4.h"
#include "KtMatrix4.h"


template<typename REAL>
class KtCamera
{
	using vec3 = KtVector3<REAL>;
	using vec4 = KtVector4<REAL>;
	using mat3 = KtMatrix3<REAL>;
	using mat4 = KtMatrix4<REAL>;

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

	bool project(const vec4& in, vec4& out) const;

	bool unproject(const vec3& in, vec4& out) const;

protected:

	// Viewing window. 
	REAL x_{ 0 }, y_{ 0 }, w_{ 1 }, h_{ 1 };

	// view matrix
	mat4 viewMatrix_;

	// standard projection matrix
	mat4 projMatrix_;
};


template<typename REAL>
KtCamera<REAL>::KtCamera()
{
	viewMatrix_ = projMatrix_ = mat4::identity();
}


template<typename REAL>
void KtCamera<REAL>::lookAt(const vec3& eye, const vec3& at, const vec3& up)
{
	vec3 zaxis = (eye - at).normalize();
	vec3 xaxis = up.cross(zaxis).normalize();
	vec3 yaxis = zaxis.cross(xaxis);

	// look at view
	viewMatrix_ = {
	   xaxis.x()          , yaxis.x()          , zaxis.x()          , 0,
	   xaxis.y()          , yaxis.y()          , zaxis.y()          , 0,
	   xaxis.z()          , yaxis.z()          , zaxis.z()          , 0,
	   -xaxis.dot(eye)    , -yaxis.dot(eye)    , -zaxis.dot(eye)    , 1
	};

	viewMatrix_.transpose(); // TODO:
}


template<typename REAL>
void KtCamera<REAL>::projectFrustum(REAL left, REAL right, REAL bottom, REAL top, REAL znear, REAL zfar)
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


template<typename REAL>
void KtCamera<REAL>::projectPerspective(REAL fovyInDegree, REAL aspectRatio, REAL znear, REAL zfar)
{
	REAL ymax, xmax;
	ymax = znear * std::tan(fovyInDegree * KtuMath<REAL>::pi / 180);
	xmax = ymax * aspectRatio;
	projectFrustum(-xmax, xmax, -ymax, ymax, znear, zfar);
}


template<typename REAL>
void KtCamera<REAL>::projectOrtho(REAL left, REAL right, REAL bottom, REAL top, REAL znear, REAL zfar)
{
	// NB: This creates 'uniform' orthographic projection matrix,
	// which depth range [-1,1], right-handed rules
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


template<typename REAL>
bool KtCamera<REAL>::project(const vec4& in, vec4& out) const
{
	out = projMatrix_ * viewMatrix_ * in;

	if (out.w() == 0.0f)
		return false;

	out.x() /= out.w();
	out.y() /= out.w();
	out.z() /= out.w();

	// map to range 0-1
	out.x() = out.x() * 0.5f + 0.5f;
	out.y() = out.y() * 0.5f + 0.5f;
	out.z() = out.z() * 0.5f + 0.5f;

	// map to viewport
	out.x() = out.x() * w_ + x_;
	out.y() = out.y() * h_ + y_;
	return true;
}


template<typename REAL>
bool KtCamera<REAL>::unproject(const vec3& in, vec4& out) const
{
	vec4 v(in);

	// map from viewport to 0-1
	v.x() = (v.x() - x_) / w_;
	v.y() = (v.y() - y_) / h_;

	// map to range -1 to 1
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