#pragma once
#include "KtArray.h"
#include "KtVector3.h"
#include "KtVector4.h"
#include "KtMatrix3.h"
#include "KtQuaternion.h"


/** Class encapsulating a standard 4x4 homogeneous matrix.
    @remarks
        uses column vectors when applying matrix multiplications,
        This means a vector is represented as a single column, 4-row
        matrix. This has the effect that the transformations implemented
        by the matrices happens right-to-left e.g. if vector V is to be
        transformed by M1 then M2 then M3, the calculation would be
        M3 * M2 * M1 * V. The order that matrices are concatenated is
        vital since matrix multiplication is not commutative, i.e. you
        can get a different result if you concatenate in the wrong order.
    @par
        The use of column vectors and right-to-left ordering is the
        standard in most mathematical texts, and is the same as used in
        OpenGL. It is, however, the opposite of Direct3D, which has
        inexplicably chosen to differ from the accepted standard and uses
        row vectors and left-to-right matrix multiplication.
    @par
        The generic form M * V which shows the layout of the matrix 
        entries is shown below:
        <pre>
            [ at(0)[0]  at(0)[1]  at(0)[2]  at(0)[3] ]   {x}
            | at(1)[0]  at(1)[1]  at(1)[2]  at(1)[3] | * {y}
            | at(2)[0]  at(2)[1]  at(2)[2]  at(2)[3] |   {z}
            [ at(3)[0]  at(3)[1]  at(3)[2]  at(3)[3] ]   {1}
        </pre>
*/

// @ROW_MAJOR: �ײ����ݵĲ���. ���KtMatrix3
template<class KReal, bool ROW_MAJOR = true>
class KtMatrix4 : public KtArray<KReal, 16>
{
	using super_ = KtArray<KReal, 16>;
	using point2 = KtPoint<KReal, 2>;
	using point3 = KtPoint<KReal, 3>;
	using vec3 = KtVector3<KReal>;
	using vec4 = KtVector4<KReal>;
	using mat3 = KtMatrix3<KReal, ROW_MAJOR>;
	using mat4 = KtMatrix4<KReal, ROW_MAJOR>;
	using quat = KtQuaternion<KReal>;

public:
	
	KtMatrix4() : super_() {}

	// ��������๹�죬��ΪKtArray��Ԫ�ع�����������¹��죬�Ӷ�������ȷ����ROW_MAJOR����
	KtMatrix4(KReal _00, KReal _01, KReal _02, KReal _03,
			 KReal _10, KReal _11, KReal _12, KReal _13,
			 KReal _20, KReal _21, KReal _22, KReal _23,
			 KReal _30, KReal _31, KReal _32, KReal _33) {
		m00() = _00, m01() = _01, m02() = _02, m03() = _03,
		m10() = _10, m11() = _11, m12() = _12, m13() = _13,
		m20() = _20, m21() = _21, m22() = _22, m23() = _23,
		m30() = _30, m31() = _31, m32() = _32, m33() = _33;
	}

	KtMatrix4(const mat3& m3) : 
		KtMatrix4(
			m3.m00(), m3.m01(), m3.m02(), 0,
			m3.m10(), m3.m11(), m3.m12(), 0,
			m3.m20(), m3.m21(), m3.m22(), 0,
			       0,        0,        0, 1 ) {}
	
	template<class T, bool R>
	KtMatrix4(const KtMatrix4<T, R>& rhs) {
		m00() = rhs.m00(), m01() = rhs.m01(), m02() = rhs.m02(), m03() = rhs.m03(),
		m10() = rhs.m10(), m11() = rhs.m11(), m12() = rhs.m12(), m13() = rhs.m13(),
		m20() = rhs.m20(), m21() = rhs.m21(), m22() = rhs.m22(), m23() = rhs.m23(),
		m30() = rhs.m30(), m31() = rhs.m31(), m32() = rhs.m32(), m33() = rhs.m33();
	}

	constexpr static bool rowMajor() { return ROW_MAJOR; }

	static mat4 zero() {
		return mat4();
	}

	static mat4 identity() {
		mat4 v;
		v.m00() = v.m11() = v.m22() = v.m33() = 1;
		return v;
	}

    /** Builds a translation matrix
    */
	static mat4 buildTanslation(const vec3& v)
    {
		return { 1.0f, 0.0f, 0.0f, v.x(),
				 0.0f, 1.0f, 0.0f, v.y(),
				 0.0f, 0.0f, 1.0f, v.z(),
				 0.0f, 0.0f, 0.0f, 1.0f };
    }

    /** Builds a scale matrix.
    */
	static mat4 buildScale(const vec3& v)
    {
		return { v.x(), 0.0f,  0.0f,  0.0f,
			     0.0f,  v.y(), 0.0f,  0.0f,
				 0.0f,  0.0f,  v.z(), 0.0f,
				 0.0f,  0.0f,  0.0f,  1.0f };
    }

	/** Builds a rotation matrix.
	*/
	static mat4 buildRotation(const quat& orient) {
		mat3 rot;
		orient.toRotateMatrix(rot);
		return rot;
	}

    /** Building a Matrix4 from orientation / scale / position.
    @remarks
        Transform is performed in the order scale, rotate, translation, i.e. translation is independent
        of orientation axes, scale does not affect size of translation, rotation and scaling are always
        centered on the origin.
    */
	static mat4 buildTransform(const vec3& pos, const vec3& scale, const quat& orient) {
		// Ordering: 1. Scale 2. Rotate 3. Translate

		mat3 rot;
		orient.toRotateMatrix(rot);

		// Set up final matrix with scale, rotation and translation
		return {

			// Set up final matrix with scale, rotation and translation
			scale.x() * rot.m00(), scale.y() * rot.m01(), scale.z() * rot.m02(), pos.x(),
			scale.x() * rot.m10(), scale.y() * rot.m11(), scale.z() * rot.m12(), pos.y(),
			scale.x() * rot.m20(), scale.y() * rot.m21(), scale.z() * rot.m22(), pos.z(),

			// No projection term
			0, 0, 0, 1
		};
	}

	/** Builds a reflection matrix.
	*/
	// @v: ����ƽ��ķ���ʸ��
	// ���=1-v*v(T)
	static mat4 buildReflection(const vec3& v)
	{
		auto x2 = v.x() * v.x(), y2 = v.y() * v.y(), z2 = v.z() * v.z();
		auto xy = v.x() * v.y(), xz = v.x() * v.z(), yz = v.y() * v.z();
		return { 1-2*x2,  -2*xy,  -2*xz, 0.0f,
				  -2*xy, 1-2*y2,  -2*yz, 0.0f,
				  -2*xz,  -2*yz, 1-2*z2, 0.0f,
				   0.0f,   0.0f,   0.0f, 1.0f };
	}

	/// ������ͼ��͸�Ӿ���

	// �����λ��eye�㣬�Ϸ�����up������at�㣬���˸���viewMatrix
	static mat4 lookAt(const vec3& eye, const vec3& at, const vec3& up);

	// ��frustum��������projMatrix_
	static mat4 projectFrustum(KReal left, KReal right, KReal bottom, KReal top, KReal znear, KReal zfar);

	static mat4 projectFrustum(const point2& lower, const point2& upper, KReal znear, KReal zfar) {
		return projectFrustum(lower.x(), upper.x(), lower.y(), upper.y(), znear, zfar);
	}

	static mat4 projectFrustum(const point3& lower, const point3& upper) {
		return projectFrustum(lower.x(), upper.x(), lower.y(), upper.y(), lower.z(), upper.z());
	}

	static mat4 projectPerspective(KReal fovyInDegree = 45, KReal aspectRatio = 4.f / 3.f, KReal znear = 100, KReal zfar = 10000);

	static mat4 projectOrtho(KReal left, KReal right, KReal bottom, KReal top, KReal znear, KReal zfar);

	static mat4 projectOrtho(const point2& lower, const point2& upper, KReal znear, KReal zfar) {
		return projectOrtho(lower.x(), upper.x(), lower.y(), upper.y(), znear, zfar);
	}

	static mat4 projectOrtho(const point3& lower, const point3& upper) {
		return projectOrtho(lower.x(), upper.x(), lower.y(), upper.y(), lower.z(), upper.z());
	}


	// ����
	mat4 operator*(const mat4& rhs) const;
	vec4 operator*(const vec4& v) const;
	vec4 operator*(const vec3& v) const;

	// ���ص�ǰ�����ת��
	mat4 getTranspose() const {
		return { m00(), m10(), m20(), m30(),
				 m01(), m11(), m21(), m31(),
				 m02(), m12(), m22(), m32(),
				 m03(), m13(), m23(), m33() };
	}

	// �õ�ǰ����Ϊ��ת��
	KtMatrix4& transpose() {
		std::swap(m01(), m10());
		std::swap(m02(), m20());
		std::swap(m03(), m30());
		std::swap(m12(), m21());
		std::swap(m13(), m31());
		std::swap(m23(), m32());

		return *this;
	}

	mat4 getInverse() const;

	mat4& inverse(); // inverse of this


    /** Check whether or not the matrix is affine matrix.
        @remarks
            An affine matrix is a 4x4 matrix with row 3 equal to (0, 0, 0, 1),
            e.g. no projective coefficients.
    */
    bool isAffine() const
    {
        return m30() == 0 && m31() == 0 && m32() == 0 && m33() == 1;
    }

	// ��ȡ
    /*
    -----------------------------------------------------------------------
    Translation Transformation
    -----------------------------------------------------------------------
    */
    /** Sets the translation transformation part of the matrix.
    */
   void setTranslation(const vec3& v) {
        m03() = v.x, m13() = v.y, m23() = v.z;
    }

    /** Extracts the translation transformation part of the matrix.
        */
    vec3 getTranslation() const {
        return vec3(m03(), m13(), m23());
    }

    /*
    -----------------------------------------------------------------------
    Scale Transformation
    -----------------------------------------------------------------------
    */

    /** Sets the scale part of the matrix.
    */
    void setScale(const vec3& v) {
        m00() = v.x(), m11() = v.y(), m22() = v.z();
    }

	/** Determines if this matrix involves a scaling. */
	bool hasScale() const {
		// check magnitude of column vectors (==local axes)
		Real t = m00() * m00() + m10() * m10() + m20() * m20();
		if (!KuMath<KReal>::almostEqual(t, 1.0))
			return true;
		t = m01() * m01() + m11() * m11() + m21() * m21();
		if (!KuMath<KReal>::almostEqual(t, 1.0))
			return true;
		t = m02() * m02() + m12() * m12() + m22() * m22();
		if (!KuMath<KReal>::almostEqual(t, 1.0))
			return true;

		return false;
	}


	/** Extracts the rotation transformation part of the matrix.
		*/
	mat3 getRotation() const {
		vec3 x(m00(), m10(), m20()), y(m01(), m11(), m21()), z(m02(), m12(), m22());
		x.normalize(), y.normalize(), z.normalize();
		return mat3(x, y, z);
	}

	// TODO: ʵ��KtMatrix���࣬ʵ�־�����ص�ͨ���㷨
	KReal operator()(unsigned row, unsigned col) const {
		if constexpr (ROW_MAJOR)
			return at(row * 4 + col);
		else
			return at(col * 4 + row);
	}

	KReal& operator()(unsigned row, unsigned col) {
		if constexpr (ROW_MAJOR)
			return at(row * 4 + col);
		else
			return at(col * 4 + row);
	}

	KReal m00() const { return at(0); }
	KReal m01() const { 
		if constexpr (ROW_MAJOR)
			return at(1);
		else
			return at(4);
	}
	KReal m02() const { 
		if constexpr (ROW_MAJOR)
			return at(2);
		else
			return at(8);
	}
	KReal m03() const { 
		if constexpr (ROW_MAJOR)
			return at(3);
		else
			return at(12);
	}

	KReal m10() const { 
		if constexpr (ROW_MAJOR)
			return at(4); 
		else
			return at(1);
	}
	KReal m11() const { return at(5); }
	KReal m12() const { 
		if constexpr (ROW_MAJOR)
			return at(6); 
		else
			return at(9);
	}
	KReal m13() const { 
		if constexpr (ROW_MAJOR)
			return at(7); 
		else
			return at(13);
	}

	KReal m20() const { 
		if constexpr (ROW_MAJOR)
			return at(8); 
		else
			return at(2);
	}
	KReal m21() const { 
		if constexpr (ROW_MAJOR)
			return at(9); 
		else
			return at(6);
	}
	KReal m22() const { return at(10); }
	KReal m23() const { 
		if constexpr (ROW_MAJOR)
			return at(11); 
		else
			return at(14);
	}

	KReal m30() const { 
		if constexpr (ROW_MAJOR)
			return at(12); 
		else
			return at(3);
	}
	KReal m31() const { 
		if constexpr (ROW_MAJOR)
			return at(13);
		else
			return at(7);
	}
	KReal m32() const { 
		if constexpr (ROW_MAJOR)
			return at(14); 
		else
			return at(11);
	}
	KReal m33() const { return at(15); }


	KReal& m00() { return at(0); }
	KReal& m01() {
		if constexpr (ROW_MAJOR)
			return at(1);
		else
			return at(4);
	}
	KReal& m02() {
		if constexpr (ROW_MAJOR)
			return at(2);
		else
			return at(8);
	}
	KReal& m03() {
		if constexpr (ROW_MAJOR)
			return at(3);
		else
			return at(12);
	}

	KReal& m10() {
		if constexpr (ROW_MAJOR)
			return at(4);
		else
			return at(1);
	}
	KReal& m11() { return at(5); }
	KReal& m12() {
		if constexpr (ROW_MAJOR)
			return at(6);
		else
			return at(9);
	}
	KReal& m13() {
		if constexpr (ROW_MAJOR)
			return at(7);
		else
			return at(13);
	}

	KReal& m20() {
		if constexpr (ROW_MAJOR)
			return at(8);
		else
			return at(2);
	}
	KReal& m21() {
		if constexpr (ROW_MAJOR)
			return at(9);
		else
			return at(6);
	}
	KReal& m22() { return at(10); }
	KReal& m23() {
		if constexpr (ROW_MAJOR)
			return at(11);
		else
			return at(14);
	}

	KReal& m30() {
		if constexpr (ROW_MAJOR)
			return at(12);
		else
			return at(3);
	}
	KReal& m31() {
		if constexpr (ROW_MAJOR)
			return at(13);
		else
			return at(7);
	}
	KReal& m32() {
		if constexpr (ROW_MAJOR)
			return at(14);
		else
			return at(11);
	}
	KReal& m33() { return at(15); }
};


template<typename KReal, bool ROW_MAJOR>
KtMatrix4<KReal, ROW_MAJOR> KtMatrix4<KReal, ROW_MAJOR>::operator*(const mat4& rhs) const
{
	return {
		m00() * rhs.m00() + m01() * rhs.m10() + m02() * rhs.m20() + m03() * rhs.m30(),
		m00() * rhs.m01() + m01() * rhs.m11() + m02() * rhs.m21() + m03() * rhs.m31(),
		m00() * rhs.m02() + m01() * rhs.m12() + m02() * rhs.m22() + m03() * rhs.m32(),
		m00() * rhs.m03() + m01() * rhs.m13() + m02() * rhs.m23() + m03() * rhs.m33(),

		m10() * rhs.m00() + m11() * rhs.m10() + m12() * rhs.m20() + m13() * rhs.m30(),
		m10() * rhs.m01() + m11() * rhs.m11() + m12() * rhs.m21() + m13() * rhs.m31(),
		m10() * rhs.m02() + m11() * rhs.m12() + m12() * rhs.m22() + m13() * rhs.m32(),
		m10() * rhs.m03() + m11() * rhs.m13() + m12() * rhs.m23() + m13() * rhs.m33(),

		m20() * rhs.m00() + m21() * rhs.m10() + m22() * rhs.m20() + m23() * rhs.m30(),
		m20() * rhs.m01() + m21() * rhs.m11() + m22() * rhs.m21() + m23() * rhs.m31(),
		m20() * rhs.m02() + m21() * rhs.m12() + m22() * rhs.m22() + m23() * rhs.m32(),
		m20() * rhs.m03() + m21() * rhs.m13() + m22() * rhs.m23() + m23() * rhs.m33(),

		m30() * rhs.m00() + m31() * rhs.m10() + m32() * rhs.m20() + m33() * rhs.m30(),
		m30() * rhs.m01() + m31() * rhs.m11() + m32() * rhs.m21() + m33() * rhs.m31(),
		m30() * rhs.m02() + m31() * rhs.m12() + m32() * rhs.m22() + m33() * rhs.m32(),
		m30() * rhs.m03() + m31() * rhs.m13() + m32() * rhs.m23() + m33() * rhs.m33()
	};
}

template<typename KReal, bool ROW_MAJOR>
KtVector4<KReal> KtMatrix4<KReal, ROW_MAJOR>::operator*(const vec4& v) const
{
	return {
		m00() * v.x() + m01() * v.y() + m02() * v.z() + m03() * v.w(),
		m10() * v.x() + m11() * v.y() + m12() * v.z() + m13() * v.w(),
		m20() * v.x() + m21() * v.y() + m22() * v.z() + m23() * v.w(),
		m30() * v.x() + m31() * v.y() + m32() * v.z() + m33() * v.w()
	};
}

template<typename KReal, bool ROW_MAJOR>
KtVector4<KReal> KtMatrix4<KReal, ROW_MAJOR>::operator*(const vec3& v) const
{
	return {
		m00() * v.x() + m01() * v.y() + m02() * v.z() + m03(),
		m10() * v.x() + m11() * v.y() + m12() * v.z() + m13(),
		m20() * v.x() + m21() * v.y() + m22() * v.z() + m23(),
		m30() * v.x() + m31() * v.y() + m32() * v.z() + m33()
	};
}

template<typename KReal, bool ROW_MAJOR> KtMatrix4<KReal, ROW_MAJOR>&
KtMatrix4<KReal, ROW_MAJOR>::inverse()
{
	KReal _00 = m00(), _01 = m01(), _02 = m02(), _03 = m03();
	KReal _10 = m10(), _11 = m11(), _12 = m12(), _13 = m13();
	KReal _20 = m20(), _21 = m21(), _22 = m22(), _23 = m23();
	KReal _30 = m30(), _31 = m31(), _32 = m32(), _33 = m33();

	KReal v0 = _20 * _31 - _21 * _30;
	KReal v1 = _20 * _32 - _22 * _30;
	KReal v2 = _20 * _33 - _23 * _30;
	KReal v3 = _21 * _32 - _22 * _31;
	KReal v4 = _21 * _33 - _23 * _31;
	KReal v5 = _22 * _33 - _23 * _32;

	KReal t00 = +(v5 * _11 - v4 * _12 + v3 * _13);
	KReal t10 = -(v5 * _10 - v2 * _12 + v1 * _13);
	KReal t20 = +(v4 * _10 - v2 * _11 + v0 * _13);
	KReal t30 = -(v3 * _10 - v1 * _11 + v0 * _12);

	KReal invDet = 1 / (t00 * _00 + t10 * _01 + t20 * _02 + t30 * _03);

	m00() = t00 * invDet;
	m10() = t10 * invDet;
	m20() = t20 * invDet;
	m30() = t30 * invDet;

	m01() = -(v5 * _01 - v4 * _02 + v3 * _03) * invDet;
	m11() = +(v5 * _00 - v2 * _02 + v1 * _03) * invDet;
	m21() = -(v4 * _00 - v2 * _01 + v0 * _03) * invDet;
	m31() = +(v3 * _00 - v1 * _01 + v0 * _02) * invDet;

	v0 = _10 * _31 - _11 * _30;
	v1 = _10 * _32 - _12 * _30;
	v2 = _10 * _33 - _13 * _30;
	v3 = _11 * _32 - _12 * _31;
	v4 = _11 * _33 - _13 * _31;
	v5 = _12 * _33 - _13 * _32;

	m02() = +(v5 * _01 - v4 * _02 + v3 * _03) * invDet;
	m12() = -(v5 * _00 - v2 * _02 + v1 * _03) * invDet;
	m22() = +(v4 * _00 - v2 * _01 + v0 * _03) * invDet;
	m32() = -(v3 * _00 - v1 * _01 + v0 * _02) * invDet;

	v0 = _21 * _10 - _20 * _11;
	v1 = _22 * _10 - _20 * _12;
	v2 = _23 * _10 - _20 * _13;
	v3 = _22 * _11 - _21 * _12;
	v4 = _23 * _11 - _21 * _13;
	v5 = _23 * _12 - _22 * _13;

	m03() = -(v5 * _01 - v4 * _02 + v3 * _03) * invDet;
	m13() = +(v5 * _00 - v2 * _02 + v1 * _03) * invDet;
	m23() = -(v4 * _00 - v2 * _01 + v0 * _03) * invDet;
	m33() = +(v3 * _00 - v1 * _01 + v0 * _02) * invDet;

	return *this;
}

template<typename KReal, bool ROW_MAJOR> KtMatrix4<KReal, ROW_MAJOR>
KtMatrix4<KReal, ROW_MAJOR>::getInverse() const
{
	mat4 m(*this);
	m.inverse();
	return m;
}


template<typename KReal, bool ROW_MAJOR> KtMatrix4<KReal, ROW_MAJOR>
KtMatrix4<KReal, ROW_MAJOR>::lookAt(const vec3& eye, const vec3& at, const vec3& up)
{
	vec3 zaxis = (eye - at).normalize(); // ���������ϵ����������ϵ��z�����෴�ģ����Դ˴�ȡ����
	vec3 xaxis = up.cross(zaxis).normalize();
	vec3 yaxis = zaxis.cross(xaxis);

	// look at view

	// ���1��Ϊeye��������������ͶӰȡ��
	// ���������������ϵ��(a, b, c)�㣬�൱����������ϵԭ�������������ϵ��(-a, -b, -c)��
	return {
	   xaxis.x(), xaxis.y(), xaxis.z(), -xaxis.dot(eye),
	   yaxis.x(), yaxis.y(), yaxis.z(), -yaxis.dot(eye),
	   zaxis.x(), zaxis.y(), zaxis.z(), -zaxis.dot(eye),
			   0,         0,         0,               1
	};
}


template<typename KReal, bool ROW_MAJOR> KtMatrix4<KReal, ROW_MAJOR>
KtMatrix4<KReal, ROW_MAJOR>::projectFrustum(KReal left, KReal right, KReal bottom, KReal top, KReal znear, KReal zfar)
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
		return mat4::zero();

	auto x = (2 * znear) / (right - left);
	auto y = (2 * znear) / (top - bottom);
	auto a = (right + left) / (right - left);
	auto b = (top + bottom) / (top - bottom);
	auto c = -(zfar + znear) / (zfar - znear);
	auto d = -(2 * zfar * znear) / (zfar - znear);

	return {
		x,    0,    a,    0,
		0,    y,    b,    0,
		0,    0,    c,    d,
		0,    0,   -1,    0
	};
}


template<typename KReal, bool ROW_MAJOR> KtMatrix4<KReal, ROW_MAJOR>
KtMatrix4<KReal, ROW_MAJOR>::projectPerspective(KReal fovyInDegree, KReal aspectRatio, KReal znear, KReal zfar)
{
	KReal ymax, xmax;
	ymax = znear * std::tan(KuMath<KReal>::deg2Rad(fovyInDegree));
	xmax = ymax * aspectRatio;
	return projectFrustum(-xmax, xmax, -ymax, ymax, znear, zfar);
}


template<typename KReal, bool ROW_MAJOR> KtMatrix4<KReal, ROW_MAJOR>
KtMatrix4<KReal, ROW_MAJOR>::projectOrtho(KReal left, KReal right, KReal bottom, KReal top, KReal znear, KReal zfar)
{
	// NB: This creates 'uniform' orthographic projection matrix,
	// which depth range [-1, +1], right-handed rules
	//
	// [ A   0   0   C  ]
	// [ 0   B   0   D  ]
	// [ 0   0   q   qn ]
	// [ 0   0   0   1  ]
	//
	// A = 2 / (right - left)
	// B = 2 / (top - bottom)
	// C = - (right + left) / (right - left)
	// D = - (top + bottom) / (top - bottom)
	// q = - 2 / (far - near)
	// qn = -(far + near) / (far - near)

	auto A = 2 / (right - left);
	auto B = 2 / (top - bottom);
	auto C = -(right + left) / (right - left);
	auto D = -(top + bottom) / (top - bottom);
	auto q = -2 / (zfar - znear);
	auto qn = -(zfar + znear) / (zfar - znear);

	return {
		A, 0, 0, C,
		0, B, 0, D,
		0, 0, q, qn,
		0, 0, 0, 1
	};
}

template<bool ROW_MAJOR = true>
using mat4f = KtMatrix4<float, ROW_MAJOR>;

template<bool ROW_MAJOR = true>
using mat4d = KtMatrix4<double, ROW_MAJOR>;

template<bool ROW_MAJOR = true>
using float4x4 = mat4f<ROW_MAJOR>;

template<bool ROW_MAJOR = true>
using double4x4 = mat4d<ROW_MAJOR>;
