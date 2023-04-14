#pragma once
#include "KtArray.h"
#include "KtVector3.h"

// ��ת����(rotation matrix)��ʵ�֣����ڽ��ֲ�����ϵ��ʸ���任����������ϵ
// ������б�ʾ�ֲ�����ϵ�е�x��y��z��������ĵ�λʸ������������ϵ�е�ͶӰ
// �ڵ��������У���ת����Ҳ�з������Ҿ���
// ��i�С���j�е�Ԫ�ر�ʾ��������ϵi��;ֲ�����ϵj��нǵ�����
// �㷨�ο�<3D Game Engine Design - A Practical Approach To Real-Time Computer Graphics>

// NOTE.  The (x,y,z) coordinate system is assumed to be right-handed.
// Coordinate axis rotation matrices are of the form
//   RX =    1       0       0
//           0     cos(t) -sin(t)
//           0     sin(t)  cos(t)
// where t > 0 indicates a counterclockwise rotation in the yz-plane
//   RY =  cos(t)    0     sin(t)
//           0       1       0
//        -sin(t)    0     cos(t)
// where t > 0 indicates a counterclockwise rotation in the zx-plane
//   RZ =  cos(t) -sin(t)    0
//         sin(t)  cos(t)    0
//           0       0       1
// where t > 0 indicates a counterclockwise rotation in the xy-plane.

// @ROW_MAJOR: �ײ����ݵĲ���
// ��true, �ײ����ݰ��д洢������˳��Ϊm[0][0], m[0][1], m[0][2], m[1][0], ...
// ��false, �ײ����ݰ��д洢������˳��Ϊm[0][0], m[1][0], m[2][0], m[0][1], ...

template<class KReal, bool ROW_MAJOR = true>
class KtMatrix3 : public KtArray<KReal, 9>
{
	using vec3 = KtVector3<KReal>;
	using mat3 = KtMatrix3<KReal, ROW_MAJOR>;
	using point3 = KtPoint<KReal, 3>;
	using super_ = KtArray<KReal, 9>;

public:

	constexpr static bool rowMajor() { return ROW_MAJOR; }

	KtMatrix3() : super_() {}

	KtMatrix3(KReal _00, KReal _01, KReal _02, 
		     KReal _10, KReal _11, KReal _12, 
		     KReal _20, KReal _21, KReal _22) {
	    m00() = _00, m01() = _01, m02() = _02,
		m10() = _10, m11() = _11, m12() = _12,
		m20() = _20, m21() = _21, m22() = _22;
	}

	// ������ʸ���������
	KtMatrix3(const vec3& c0, const vec3& c1, const vec3& c2) {
		m00() = c0.y(); m10() = c0.y(); m20() = c0.z();
		m01() = c1.x(); m11() = c1.y(); m21() = c1.z();
		m02() = c2.x(); m12() = c2.y(); m22() = c2.z();
	}


	vec3 xAxis() const { return { m00(), m10(), m20() }; } // ��1��
	vec3 yAxis() const { return { m01(), m11(), m21() }; } // ��2��
	vec3 zAxis() const { return { m02(), m12(), m22() }; } // ��3��

	static mat3 zero() {
		return mat3();
	}

	static mat3 identity() {
		mat3 v();
		v.m00() = v.m11() = v.m22() = 1;
		return v;
	}

	mat3& operator=(const mat3& rhs) {
		std::copy(std::cbegin(rhs), std::cend(rhs), super_::begin());
		return *this;
	}

	bool operator==(const mat3& rhs) const;
	bool isApproxEqual(const mat3& rhs) const;

	mat3 operator-();
	mat3 operator*(const mat3& rhs) const;
	vec3 operator*(const vec3& v) const;

	mat3& transpose() // �õ�ǰ����Ϊ��ת��
	{
		std::swap(m01(), m10());
		std::swap(m02(), m20());
		std::swap(m12(), m21());

		return *this;
	}

	void orthonormalize(); // ������

	// ����ת��-��ת��֮���ת��
	mat3& fromAngleAxis(KReal angle/*����*/, const vec3& axis); // ������ת�����ת�Ƕȹ�����ת����
	void toAngleAxis(KReal& angle/*����*/, vec3& axis); // ��ȡ��ת�����е���ת�����ת�Ƕ�
		 
	// ��ŷ����֮���ת��
	// ��ת������ϵΪ�ֲ�����ϵ���ֲ�����ϵ����Ӧ��˳����ת�ض���ŷ���Ǻ�ͬ��������ϵ�غ�
	// ����ת����ԭ�㿴��˳ʱ��Ϊ����
	mat3& fromEulerAngleX(KReal angleX); // ������(��������ϵ)X����תangle���ȵ���ת����
	mat3& fromEulerAngleY(KReal angleY); // ������(��������ϵ)Y����תangle���ȵ���ת����
	mat3& fromEulerAngleZ(KReal angleZ); // ������(��������ϵ)Z����תangle���ȵ���ת����

	// ����Ϊ��������
	void toEulerAngleXYZ(KReal& angleX, KReal& angleY, KReal& angleZ) const; // �ֲ���������Z����תangleZ��Ȼ�����µ�Y����תangleY��������µ�X����תangleX������������ϵ�غϡ�
	void toEulerAngleXZY(KReal& angleX, KReal& angleY, KReal& angleZ) const;
	void toEulerAngleYXZ(KReal& angleX, KReal& angleY, KReal& angleZ) const; 
	void toEulerAngleYZX(KReal& angleX, KReal& angleY, KReal& angleZ) const;
	void toEulerAngleZXY(KReal& angleX, KReal& angleY, KReal& angleZ) const;
	void toEulerAngleZYX(KReal& angleX, KReal& angleY, KReal& angleZ) const;

	void toEulerAngleXYZ(point3& v) const { toEulerAngleXYZ(v.x(), v.y(), v.z()); }
	void toEulerAngleXZY(point3& v) const { toEulerAngleXZY(v.x(), v.y(), v.z()); }
	void toEulerAngleYXZ(point3& v) const { toEulerAngleYXZ(v.x(), v.y(), v.z()); }
	void toEulerAngleYZX(point3& v) const { toEulerAngleYZX(v.x(), v.y(), v.z()); }
	void toEulerAngleZXY(point3& v) const { toEulerAngleZXY(v.x(), v.y(), v.z()); }
	void toEulerAngleZYX(point3& v) const { toEulerAngleZYX(v.x(), v.y(), v.z()); }

	mat3& fromEulerAngleXYZ(KReal angleX, KReal angleY, KReal angleZ);
	mat3& fromEulerAngleXZY(KReal angleX, KReal angleY, KReal angleZ);
	mat3& fromEulerAngleYXZ(KReal angleX, KReal angleY, KReal angleZ);
	mat3& fromEulerAngleYZX(KReal angleX, KReal angleY, KReal angleZ);
	mat3& fromEulerAngleZXY(KReal angleX, KReal angleY, KReal angleZ);
	mat3& fromEulerAngleZYX(KReal angleX, KReal angleY, KReal angleZ);

	mat3& fromEulerAngleXYZ(const point3& v) { return fromEulerAngleXYZ(v.x(), v.y(), v.z()); }
	mat3& fromEulerAngleXZY(const point3& v) { return fromEulerAngleXZY(v.x(), v.y(), v.z()); }
	mat3& fromEulerAngleYXZ(const point3& v) { return fromEulerAngleYXZ(v.x(), v.y(), v.z()); }
	mat3& fromEulerAngleYZX(const point3& v) { return fromEulerAngleYZX(v.x(), v.y(), v.z()); }
	mat3& fromEulerAngleZXY(const point3& v) { return fromEulerAngleZXY(v.x(), v.y(), v.z()); }
	mat3& fromEulerAngleZYX(const point3& v) { return fromEulerAngleZYX(v.x(), v.y(), v.z()); }

	mat3& fromYawPitchRoll(KReal yaw, KReal pitch, KReal roll) {
		return fromEulerAngleYXZ(pitch, yaw, roll); 
	}

	KReal m00() const {	return at(0); }
	KReal m01() const { 
		if constexpr (rowMajor()) return at(1);
		else return at(3);
	}	
	KReal m02() const { 
		if constexpr (rowMajor()) return at(2);
		else return at(6);
	 }	
	KReal m10() const { 
		if constexpr (rowMajor()) return at(3);
		else return at(1);
	}
	KReal m11() const { return at(4); }
	KReal m12() const { 
		if constexpr (rowMajor()) return at(5);
		else return at(7);
	}
	KReal m20() const { 
		if constexpr (rowMajor()) return at(6);
		else return at(2);
	}
	KReal m21() const { 
		if constexpr (rowMajor()) return at(7);
		else return at(5);
	}
	KReal m22() const { return at(8); }
	
	KReal& m00() { return at(0); }
	KReal& m01() {
		if constexpr (rowMajor()) return at(1);
		else return at(3);
	}
	KReal& m02() {
		if constexpr (rowMajor()) return at(2);
		else return at(6);
	}
	KReal& m10() {
		if constexpr (rowMajor()) return at(3);
		else return at(1);
	}
	KReal& m11() { return at(4); }
	KReal& m12() {
		if constexpr (rowMajor()) return at(5);
		else return at(7);
	}
	KReal& m20() {
		if constexpr (rowMajor()) return at(6);
		else return at(2);
	}
	KReal& m21() {
		if constexpr (rowMajor()) return at(7);
		else return at(5);
	}
	KReal& m22() { return at(8); }
};

template<class KReal, bool ROW_MAJOR>
bool KtMatrix3<KReal, ROW_MAJOR>::operator==(const mat3& rhs) const
{
	return m00() == rhs.m00() && m01() == rhs.m01() && m02() == rhs.m02() &&
		m10() == rhs.m10() && m11() == rhs.m11() && m12() == rhs.m12() &&
		m20() == rhs.m20() && m21() == rhs.m21() && m22() == rhs.m22();
}

template<class KReal, bool ROW_MAJOR>
bool KtMatrix3<KReal, ROW_MAJOR>::isApproxEqual(const mat3& rhs) const
{
	return KuMath::approxEqual(m00(), rhs.m00()) && KuMath::approxEqual(m01(), rhs.m01()) && KuMath::approxEqual(m02(), rhs.m02()) &&
		KuMath::approxEqual(m10(), rhs.m10()) && KuMath::approxEqual(m11(), rhs.m11()) && KuMath::approxEqual(m12(), rhs.m12()) &&
		KuMath::approxEqual(m20(), rhs.m20()) && KuMath::approxEqual(m21(), rhs.m21()) && KuMath::approxEqual(m22(), rhs.m22());
}

template<class KReal, bool ROW_MAJOR>
KtMatrix3<KReal, ROW_MAJOR> KtMatrix3<KReal, ROW_MAJOR>::operator-()
{
	mat3 r;
	for (int i = 0; i < 9; i++)
		r.m_[i] = -m_[i];

	return r;
}

// ��ά����˷�
template<class KReal, bool ROW_MAJOR>
KtMatrix3<KReal, ROW_MAJOR> KtMatrix3<KReal, ROW_MAJOR>::operator*(const mat3& rhs) const
{
	mat3 r;
	r.m00() = m00() * rhs.m00() + m01() * rhs.m10() + m02() * rhs.m20();
	r.m01() = m00() * rhs.m01() + m01() * rhs.m11() + m02() * rhs.m21();
	r.m02() = m00() * rhs.m02() + m01() * rhs.m12() + m02() * rhs.m22();

	r.m10() = m10() * rhs.m00() + m11() * rhs.m10() + m12() * rhs.m20();
	r.m11() = m10() * rhs.m01() + m11() * rhs.m11() + m12() * rhs.m21();
	r.m12() = m10() * rhs.m02() + m11() * rhs.m12() + m12() * rhs.m22();

	r.m20() = m20() * rhs.m00() + m21() * rhs.m10() + m22() * rhs.m20();
	r.m21() = m20() * rhs.m01() + m21() * rhs.m11() + m22() * rhs.m21();
	r.m22() = m20() * rhs.m02() + m21() * rhs.m12() + m22() * rhs.m22();

	return r;
}

template<class KReal, bool ROW_MAJOR> KtVector3<KReal> 
KtMatrix3<KReal, ROW_MAJOR>::operator*(const vec3& v) const
{
	auto x = m00() * v.x() + m01() * v.y() + m02() * v.z();
	auto y = m10() * v.x() + m11() * v.y() + m12() * v.z();
	auto z = m20() * v.x() + m21() * v.y() + m22() * v.z();
	return { x, y, z };
}

// TODO: ....
template<class KReal, bool ROW_MAJOR>
void KtMatrix3<KReal, ROW_MAJOR>::orthonormalize()
{
	// compute q0
	KReal fInvLength = 1 / std::sqrt(m00() * m00()
		+ m10() * m10() +
		m20() * m20());

	m00() *= fInvLength;
	m10() *= fInvLength;
	m20() *= fInvLength;

	// compute q1
	KReal fDot0 =
		m00() * m01() +
		m10() * m11() +
		m20() * m21();

	m01() -= fDot0 * m00();
	m11() -= fDot0 * m10();
	m21() -= fDot0 * m20();

	fInvLength = 1 / std::sqrt(m01() * m01() +
		m11() * m11() +
		m21() * m21());

	m01() *= fInvLength;
	m11() *= fInvLength;
	m21() *= fInvLength;

	// compute q2
	KReal fDot1 =
		m01() * m02() +
		m11() * m12() +
		m21() * m22();

	fDot0 =
		m00() * m02() +
		m10() * m12() +
		m20() * m22();

	m02() -= fDot0 * m00() + fDot1 * m01();
	m12() -= fDot0 * m10() + fDot1 * m11();
	m22() -= fDot0 * m20() + fDot1 * m21();

	fInvLength = 1 / std::sqrt(m02() * m02() +
		m12() * m12() +
		m22() * m22());

	m02() *= fInvLength;
	m12() *= fInvLength;
	m22() *= fInvLength;
}

// ����ת��U����ת�Ƕ�A���Թ�����ת����R���£�
// R = I+sin(A)*S+(1-cos(A))*S^2
// ����IΪ��λ��S�Ĺ������£�
//     |  0  -u2   u1 |
// S = | u2   0   -u0 |
//     |-u1  u0     0 |
// ���A>0, R��ʾ��U����ʱ��(˳��U��ָ���һ����U��ԭ�㿴)��תA���ȵľ���
// ͨ��Ԥ�ȼ���S^2���Լ�����axisΪ��λʸ�������ԣ���������㷨Ч�ʣ�
//       | -u2^2-u1^2        u0u1        u0u2 |   | u0^2-1    u0u1    u0u2 |
// S^2 = |       u0u1  -u2^2-u0^2        u1u2 | = |   u0u1  u1^2-1    u1u2 |
//       |       u0u2        u1u2  -u1^2-u0^2 |   |   u0u2    u1u2  u2^2-1 |
template<class KReal, bool ROW_MAJOR> KtMatrix3<KReal, ROW_MAJOR>& 
KtMatrix3<KReal, ROW_MAJOR>::fromAngleAxis(KReal angle, const vec3& axis)
{
	KReal fCos = std::cos(angle);
	KReal fSin = std::sin(angle);
	KReal fOneMinusCos = 1.0f - fCos;
	KReal fX2 = axis.x() * axis.x();
	KReal fY2 = axis.y() * axis.y();
	KReal fZ2 = axis.z() * axis.z();
	KReal fXYM = axis.x() * axis.y() * fOneMinusCos;
	KReal fXZM = axis.x() * axis.z() * fOneMinusCos;
	KReal fYZM = axis.y() * axis.z() * fOneMinusCos;
	KReal fXSin = axis.x() * fSin;
	KReal fYSin = axis.y() * fSin;
	KReal fZSin = axis.z() * fSin;

	m00() = fX2 * fOneMinusCos + fCos;
	m01() = fXYM - fZSin;
	m02() = fXZM + fYSin;
	m10() = fXYM + fZSin;
	m11() = fY2 * fOneMinusCos + fCos;
	m12() = fYZM - fXSin;
	m20() = fXZM - fYSin;
	m21() = fYZM + fXSin;
	m22() = fZ2 * fOneMinusCos + fCos;

	return *this;
}

// cos(A) = (trace(R)-1)/2    (1)
// R-RT = 2sin(A)S            (2)
// ����ʽ(1)������ȡ��ת�Ƕ�A
// ��A=0ʱ����ת���Ϊ������
// ��A��(0, PI)����ʱ������ʽ(2)�ɵã���ת��U = V/|V|, ����V = (r21-r12, r02-r20, r10-r01)
// ��A=PIʱ��R-RT = 0����
//              | 1-2(u1^2+u2^2)           2u0u1           2u0u2 |
// R = I+2S^2 = |          2u0u1  1-2(u0^2+u2^2)           2u1u2 |
//              |          2u0u2           2u1u2  1-2(u0^2+u1^2) |
// ���r00Ϊ�Խ�������ֵ����ôu0Ϊ��ת���������������ֵ���У�
// 4u0^2 = r00-r11-r22+1 ==> u0 = sqrt(r00-r11-r22+1)/2 ==> u1 = r01/(2u0), u2 = r02/(2u0)
// ���Ƶģ����r11Ϊ�Խ�������ֵ����ô��
// u1 = sqrt(r11-r00-r22+1)/2, u0 = r01/(2u1), u2 = r12/(2u1)
// ���r22Ϊ�Խ�������ֵ����ô��
// u2 = sqrt(r22-r00-r11+1)/2, u0 = r02/(2u2), u1 = r12/(2u2)
template<class KReal, bool ROW_MAJOR>
void KtMatrix3<KReal, ROW_MAJOR>::toAngleAxis(KReal& angle/*out*/, vec3& axis/*out*/)
{
	KReal fTrace = m00() + m11() + m22();
	KReal fCos = 0.5f * (fTrace - 1.0f);
	angle = std::acos(fCos);

	if (angle > 0)
	{
		if (angle < KuMath::pi)
		{// angle��(0, PI)����
			axis.x() = m21() - m12();
			axis.y() = m02() - m20();
			axis.z() = m10() - m01();
			axis.normalize();
		}
		else
		{// angle = PI
			if (m00() > m11() && m00() > m22())
			{// m00Ϊ���ֵ
				axis.x() = std::sqrt(m00() - m11() - m22() + 1) * 0.5f;
				KReal f = 0.5f / axis.x();
				axis.y() = m01() * f;
				axis.z() = m02() * f;
			}
			else if (m11() > m22()) // m00 <= m11
			{// m11Ϊ���ֵ
				axis.y() = std::sqrt(m11() - m00() - m22() + 1) * 0.5f;
				KReal f = 0.5f / axis.y();
				axis.x() = m01() * f;
				axis.z() = m12() * f;
			}
			else
			{// m22Ϊ���ֵ
				axis.z() = std::sqrt(m22() - m00() - m11() + 1) * 0.5f;
				KReal f = 0.5f / axis.z();
				axis.x() = m02() * f;
				axis.y() = m12() * f;
			}
		}
	}
	else
	{// angle = 0, ��ת���Ϊ���ⵥλ������ȡX��
		axis.x() = 1.0f;
		axis.y() = 0.0f;
		axis.z() = 0.0f;
	}
}

template<class KReal, bool ROW_MAJOR> KtMatrix3<KReal, ROW_MAJOR>& 
KtMatrix3<KReal, ROW_MAJOR>::fromEulerAngleX(KReal angleX)
{
	KReal fCos = std::cos(angleX);
	KReal fSin = std::sin(angleX);

	m00() = 1.0f, m01() = 0.0f, m02() = 0.0f,
	m10() = 0.0f, m11() = fCos, m12() = -fSin,
	m20() = 0.0f, m21() = fSin, m22() = fCos;

	return *this;
}

template<class KReal, bool ROW_MAJOR> KtMatrix3<KReal, ROW_MAJOR>& 
KtMatrix3<KReal, ROW_MAJOR>::fromEulerAngleY(KReal angleY)
{
	KReal fCos = std::cos(angleY);
	KReal fSin = std::sin(angleY);

	m00() = fCos, m01() = 0.0f, m02() = fSin,
    m10() = 0.0f, m11() = 1.0f, m12() = 0.0f,
	m20() = -fSin, m21() = 0.0f, m22() = fCos;

	return *this;
}

template<class KReal, bool ROW_MAJOR> KtMatrix3<KReal, ROW_MAJOR>& 
KtMatrix3<KReal, ROW_MAJOR>::fromEulerAngleZ(KReal angleZ)
{
	KReal fCos = std::cos(angleZ);
	KReal fSin = std::sin(angleZ);

	m00() = fCos, m01() = -fSin, m02() = 0.0f,
	m10() = fSin, m11() = fCos, m12() = 0.0f,
	m20() = 0.0f, m21() = 0.0f, m22() = 1.0f;

	return *this;
}

// ����ǰ��ת�����ʾΪRx*Ry*Rz����ʽ��
// ���� Rx = FromEulerAngleX(angleX)
//      Ry = FromEulerAngleY(angleY)
//      Rz = FromEulerAngleZ(angleZ)
template<class KReal, bool ROW_MAJOR>
void KtMatrix3<KReal, ROW_MAJOR>::toEulerAngleXYZ(KReal& angleX, KReal& angleY, KReal& angleZ) const
{
	//     |       cy*cz         -cy*sz            sy |
	// R = |  cz*sx*sy+cx*sz   cx*cz-sx*sy*sz  -cy*sx |
	//     | -cx*cz*sy+sx*sz   cz*sx+cx*sy*sz   cx*cy |

	angleY = std::asin(m02()); // ��ΪR[0][2] = sy = sin(angleY)
	if (angleY < KuMath::pi / 2) {
		if (angleY > -KuMath::pi / 2) {
			angleX = std::atan2(-m12(), m22());
			angleZ = std::atan2(-m01(), m00());
		}
		else {
			// not a unique solution
			angleX = -std::atan2(m10(), m11());
			angleZ = 0;
		}
	}
	else {
		// not a unique solution
		angleX = std::atan2(m10(), m11());
		angleZ = 0;
	}
}

// ����ǰ��ת�����ʾΪRx*Rz*Ry����ʽ��
template<class KReal, bool ROW_MAJOR>
void KtMatrix3<KReal, ROW_MAJOR>::toEulerAngleXZY(KReal& angleX, KReal& angleY, KReal& angleZ) const
{
	//     |      cy*cz         -sz              cz*sy |
	// R = |  sx*sy+cx*cy*sz   cx*cz   -cy*sx+cx*sy*sz |
	//     | -cx*sy+cy*sx*sz   cz*sx    cx*cy+sx*sy*sz |

	angleZ = std::asin(-m01()); // ��ΪR[0][1] = -sz = -sin(angleY)
	if (angleZ < KuMath::pi / 2) {
		if (angleZ > -KuMath::pi / 2) {
			angleX = std::atan2(m21(), m11());
			angleY = std::atan2(m02(), m00());
		}
		else {
			// not a unique solution
			angleX = -std::atan2(-m20(), m22());
			angleY = 0;
		}
	}
	else {
		// not a unique solution
		angleX = std::atan2(-m20(), m22());
		angleY = 0;
	}
}

// ����ǰ��ת�����ʾΪRy*Rx*Rz����ʽ��
template<class KReal, bool ROW_MAJOR>
void KtMatrix3<KReal, ROW_MAJOR>::toEulerAngleYXZ(KReal& angleX, KReal& angleY, KReal& angleZ) const
{
	//     |  cy*cz+sx*sy*sz   cz*sx*sy-cy*sz    cx*sy |
	// R = |       cx*sz           cx*cz           -sx |
	//     | -cz*sy+cy*sx*sz   cy*cz*sx+sy*sz    cx*cy |

	angleX = std::asin(-m12()); // ��ΪR[1][2] = -sx = -sin(angleX)
	if (angleX < KuMath::pi / 2) {
		if (angleX > -KuMath::pi / 2) {
			angleY = std::atan2(m02(), m22());
			angleZ = std::atan2(m10(), m11());
		}
		else {
			// not a unique solution
			angleY = -std::atan2(-m01(), m00());
			angleZ = 0;
		}
	}
	else {
		// not a unique solution
		angleY = std::atan2(-m01(), m00());
		angleZ = 0;
	}
}

// ����ǰ��ת�����ʾΪRy*Rz*Rx����ʽ��
template<class KReal, bool ROW_MAJOR>
void KtMatrix3<KReal, ROW_MAJOR>::toEulerAngleYZX(KReal& angleX, KReal& angleY, KReal& angleZ) const
{
	//     |  cy*cz     sx*sy-cx*cy*sz    cx*sy+cy*sx*sz |
	// R = |    sz           cx*cz                -cz*sx |
	//     | -cz*sy     cy*sx+cx*sy*sz    cx*cy-sx*sy*sz |

	angleZ = std::asin(m10()); // ��ΪR[1][0] = sz = sin(angleZ)
	if (angleZ < KuMath::pi / 2) {
		if (angleZ > -KuMath::pi / 2) {
			angleY = std::atan2(-m20(), m00());
			angleX = std::atan2(-m12(), m11());
		}
		else {
			// not a unique solution
			angleY = -std::atan2(m21(), m22());
			angleX = 0;
		}
	}
	else {
		// not a unique solution
		angleY = std::atan2(m21(), m22());
		angleX = 0;
	}
}

// ����ǰ��ת�����ʾΪRz*Rx*Ry����ʽ��
template<class KReal, bool ROW_MAJOR>
void KtMatrix3<KReal, ROW_MAJOR>::toEulerAngleZXY(KReal& angleX, KReal& angleY, KReal& angleZ) const
{
	//     | cy*cz-sx*sy*sz     -cx*sz    cz*sy+cy*sx*sz |
	// R = | cz*sx*sy+cy*sz      cx*cz   -cy*cz*sx+sy*sz |
	//     |     -cx*sy            sx              cx*cy |

	angleX = std::asin(m21()); // ��ΪR[2][1] = sx = sin(angleX)
	if (angleX < KuMath::pi / 2) {
		if (angleX > -KuMath::pi / 2) {
			angleZ = std::atan2(-m01(), m11());
			angleY = std::atan2(-m20(), m22());
		}
		else {
			// not a unique solution
			angleZ = -std::atan2(m02(), m00());
			angleY = 0;
		}
	}
	else {
		// not a unique solution
		angleZ = std::atan2(m02(), m00());
		angleY = 0;
	}
}

// ����ǰ��ת�����ʾΪRz*Ry*Rx����ʽ��
template<class KReal, bool ROW_MAJOR>
void KtMatrix3<KReal, ROW_MAJOR>::toEulerAngleZYX(KReal& angleX, KReal& angleY, KReal& angleZ) const
{
	//     | cy*cz     cz*sx*sy-cx*sz    cx*cz*sy+sx*sz |
	// R = | cy*sz     cx*cz+sx*sy*sz   -cz*sx+cx*sy*sz |
	//     | -sy           cy*sx                  cx*cy |

	angleY = std::asin(-m20()); // ��ΪR[2][0] = -sy = -sin(angleY)
	if (angleY < KuMath::pi / 2) {
		if (angleY > -KuMath::pi / 2) {
			angleZ = std::atan2(m10(), m00());
			angleX = std::atan2(m21(), m22());
		}
		else {
			// not a unique solution
			angleZ = -std::atan2(-m01(), m02());
			angleX = 0;
		}
	}
	else {
		// not a unique solution
		angleZ = std::atan2(-m01(), m02());
		angleX = 0;
	}
}

template<class KReal, bool ROW_MAJOR> KtMatrix3<KReal, ROW_MAJOR>& 
KtMatrix3<KReal, ROW_MAJOR>::fromEulerAngleXYZ(KReal angleX, KReal angleY, KReal angleZ)
{
	mat3 kMatX, kMatY, kMatZ;
	kMatX.fromEulerAngleX(angleX);
	kMatY.fromEulerAngleY(angleY);
	kMatZ.fromEulerAngleZ(angleZ);

	return *this = kMatX * (kMatY * kMatZ);
}

template<class KReal, bool ROW_MAJOR> KtMatrix3<KReal, ROW_MAJOR>& 
KtMatrix3<KReal, ROW_MAJOR>::fromEulerAngleXZY(KReal angleX, KReal angleY, KReal angleZ)
{
	mat3 kMatX, kMatY, kMatZ;
	kMatX.fromEulerAngleX(angleX);
	kMatY.fromEulerAngleY(angleY);
	kMatZ.fromEulerAngleZ(angleZ);

	return *this = kMatX * (kMatZ * kMatY);
}

template<class KReal, bool ROW_MAJOR> KtMatrix3<KReal, ROW_MAJOR>&
KtMatrix3<KReal, ROW_MAJOR>::fromEulerAngleYXZ(KReal angleX, KReal angleY, KReal angleZ)
{
	mat3 kMatX, kMatY, kMatZ;
	kMatX.fromEulerAngleX(angleX);
	kMatY.fromEulerAngleY(angleY);
	kMatZ.fromEulerAngleZ(angleZ);

	return *this = kMatY * (kMatX * kMatZ);
}

template<class KReal, bool ROW_MAJOR> KtMatrix3<KReal, ROW_MAJOR>& 
KtMatrix3<KReal, ROW_MAJOR>::fromEulerAngleYZX(KReal angleX, KReal angleY, KReal angleZ)
{
	mat3 kMatX, kMatY, kMatZ;
	kMatX.fromEulerAngleX(angleX);
	kMatY.fromEulerAngleY(angleY);
	kMatZ.fromEulerAngleZ(angleZ);

	return *this = kMatY * (kMatZ * kMatX);
}

template<class KReal, bool ROW_MAJOR> KtMatrix3<KReal, ROW_MAJOR>& 
KtMatrix3<KReal, ROW_MAJOR>::fromEulerAngleZXY(KReal angleX, KReal angleY, KReal angleZ)
{
	mat3 kMatX, kMatY, kMatZ;
	kMatX.fromEulerAngleX(angleX);
	kMatY.fromEulerAngleY(angleY);
	kMatZ.fromEulerAngleZ(angleZ);

	return *this = kMatZ * (kMatX * kMatY);
}

template<class KReal, bool ROW_MAJOR> KtMatrix3<KReal, ROW_MAJOR>& 
KtMatrix3<KReal, ROW_MAJOR>::fromEulerAngleZYX(KReal angleX, KReal angleY, KReal angleZ)
{
	mat3 kMatX, kMatY, kMatZ;
	kMatX.fromEulerAngleX(angleX);
	kMatY.fromEulerAngleY(angleY);
	kMatZ.fromEulerAngleZ(angleZ);

	return *this = kMatZ * (kMatY * kMatX);
}

template<bool ROW_MAJOR = true>
using mat3f = KtMatrix3<float, ROW_MAJOR>;

template<bool ROW_MAJOR = true>
using mat3d = KtMatrix3<double, ROW_MAJOR>;

template<bool ROW_MAJOR = true>
using float3x3 = mat3f<ROW_MAJOR>;

template<bool ROW_MAJOR = true>
using double3x3 = mat3d<ROW_MAJOR>;
