#pragma once
#include <memory.h>
#include "KtVector4.h"
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
            [ m[0][0]  m[0][1]  m[0][2]  m[0][3] ]   {x}
            | m[1][0]  m[1][1]  m[1][2]  m[1][3] | * {y}
            | m[2][0]  m[2][1]  m[2][2]  m[2][3] |   {z}
            [ m[3][0]  m[3][1]  m[3][2]  m[3][3] ]   {1}
        </pre>
*/

// @ROW_MAJOR: 底层数据的布局. 详见KtMatrix3
template<class KReal, bool ROW_MAJOR = true>
class KtMatrix4  
{
public:
	// 构造
	KtMatrix4();

	KtMatrix4(KReal _00, KReal _01, KReal _02, KReal _03,
			 KReal _10, KReal _11, KReal _12, KReal _13,
			 KReal _20, KReal _21, KReal _22, KReal _23,
			 KReal _30, KReal _31, KReal _32, KReal _33) 
	{
		m00() = _00, m01() = _01, m02() = _02, m03() = _03,
		m10() = _10, m11() = _11, m12() = _12, m13() = _13,
		m20() = _20, m21() = _21, m22() = _22, m23() = _23,
		m30() = _30, m31() = _31, m32() = _32, m33() = _33;
	}
		
    /** Builds a translation matrix
    */
    void MakeTranslation( const KtVector3<KReal>& v )
    {
        m00() = 1.0; m01() = 0.0; m02() = 0.0; m03() = v.x;
        m10() = 0.0; m11() = 1.0; m12() = 0.0; m13() = v.y;
        m20() = 0.0; m21() = 0.0; m22() = 1.0; m23() = v.z;
        m30() = 0.0; m31() = 0.0; m32() = 0.0; m33() = 1.0;
    }

    /** Builds a scale matrix.
    */
	void MakeScale( const KtVector3<KReal>& v )
    {
        m00() = v.x;  m01() = 0.0;  m02() = 0.0;  m03() = 0.0;
		m10() = 0.0;  m11() = v.y;  m12() = 0.0;  m13() = 0.0;
		m20() = 0.0;  m21() = 0.0;  m22() = v.z;  m23() = 0.0;
		m30() = 0.0;  m31() = 0.0;  m32() = 0.0;  m33() = 1.0;
    }

    /** Building a Matrix4 from orientation / scale / position.
    @remarks
        Transform is performed in the order scale, rotate, translation, i.e. translation is independent
        of orientation axes, scale does not affect size of translation, rotation and scaling are always
        centered on the origin.
    */
    void MakeTransform(const KtVector3<KReal>& kvPos, const KtVector3<KReal>& kvScale, const KtQuaternion<KReal>& kvOrientation);

	KtMatrix4& operator=(const KtMatrix4& rhs) { memcpy(m, rhs.m, 16*sizeof(KReal)); return *this; }
	KtMatrix4& operator=(const KtMatrix3<KReal>& rhs);	
	
	// 比较
	bool operator==(const KtMatrix4& rhs) const;
	bool IsApproxEqual(const KtMatrix4& rhs) const;

	// 运算
	KtMatrix4 operator*(const KtMatrix4& rhs) const;
	KtVector4<KReal> operator*(const KtVector4<KReal>& v) const;
	KtVector4<KReal> operator*(const KtVector3<KReal>& v) const;

	KtMatrix4 GetTranspose() const // 返回当前矩阵的转置
	{
		return KtMatrix4(m00(), m10(), m20(), m30(),
						m01(), m11(), m21(), m31(),
						m02(), m12(), m22(), m32(),
						m03(), m13(), m23(), m33());
	}

	KtMatrix4& Transpose() // 置当前矩阵为其转置
	{
		KtuMath<KReal>::Swap(m01(), m10());
		KtuMath<KReal>::Swap(m02(), m20());
		KtuMath<KReal>::Swap(m03(), m30());
		KtuMath<KReal>::Swap(m12(), m21());
		KtuMath<KReal>::Swap(m13(), m31());
		KtuMath<KReal>::Swap(m23(), m32());

		return *this;
	}

	KtMatrix4& SetTransposeOf(const KtMatrix4& m) // 计算m的转置，结果存储在当前矩阵中
	{
		m00() = m.m00(), m01() = m.m10(), m02() = m.m20(), m03() = m.m30(),
		m10() = m.m01(), m11() = m.m11(), m12() = m.m21(), m13() = m.m31(),
		m20() = m.m02(), m21() = m.m12(), m22() = m.m22(), m23() = m.m32(),
		m30() = m.m03(), m31() = m.m13(), m32() = m.m23(), m33() = m.m33();	

		return *this;
	}

	KtMatrix4 GetInverse() const; // return the inverse matrix of this
	KtMatrix4& SetInverseOf(const KtMatrix4& m); // this = inverse of m
	KtMatrix4& Inverse(); // inverse this
//	void Inverse0(const KtMatrix4& m); // this = inverse of m

	KReal operator[](int nIndex) const { return m[nIndex]; } 
	KReal& operator[](int nIndex) { return m[nIndex]; } 

    /** Check whether or not the matrix is affine matrix.
        @remarks
            An affine matrix is a 4x4 matrix with row 3 equal to (0, 0, 0, 1),
            e.g. no projective coefficients.
    */
    bool IsAffine(void) const
    {
        return m30() == 0 && m31() == 0 && m32() == 0 && m33() == 1;
    }

	// 提取
    /*
    -----------------------------------------------------------------------
    Translation Transformation
    -----------------------------------------------------------------------
    */
    /** Sets the translation transformation part of the matrix.
    */
   void SetTranslation( const KtVector3<KReal>& v )
    {
        m03() = v.x, m13() = v.y, m23() = v.z;
    }

    /** Extracts the translation transformation part of the matrix.
        */
    KtVector3<KReal> GetTranslation() const
    {
        return KtVector3<KReal>(m03(), m13(), m23());
    }

    /*
    -----------------------------------------------------------------------
    Scale Transformation
    -----------------------------------------------------------------------
    */
    /** Sets the scale part of the matrix.
    */
    void SetScale( const KtVector3<KReal>& v )
    {
        m00() = v.x, m11() = v.y, m22() = v.z;
    }

	/** Determines if this matrix involves a scaling. */
	bool HasScale() const
	{
		// check magnitude of column vectors (==local axes)
		Real t = m00() * m00() + m10() * m10() + m20() * m20();
		if (!KtuMath<KReal>::ApproxEqual(t, 1.0))
			return true;
		t = m01() * m01() + m11() * m11() + m21() * m21();
		if (!KtuMath<KReal>::ApproxEqual(t, 1.0))
			return true;
		t = m02() * m02() + m12() * m12() + m22() * m22();
		if (!KtuMath<KReal>::ApproxEqual(t, 1.0))
			return true;

		return false;
	}

#ifdef K_COLUMN_MAJOR  // 列主序
	KReal m00() const { return m[0]; }
	KReal m10() const { return m[1]; }
	KReal m20() const { return m[2]; }
	KReal m30() const { return m[3]; }

	KReal m01() const { return m[4]; }
	KReal m11() const { return m[5]; }
	KReal m21() const { return m[6]; }
	KReal m31() const { return m[7]; }

	KReal m02() const { return m[8]; }
	KReal m12() const { return m[9]; }
	KReal m22() const { return m[10]; }
	KReal m32() const { return m[11]; }

	KReal m03() const { return m[12]; }
	KReal m13() const { return m[13]; }
	KReal m23() const { return m[14]; }
	KReal m33() const { return m[15]; }

	KReal& m00() { return m[0]; }
	KReal& m10() { return m[1]; }
	KReal& m20() { return m[2]; }
	KReal& m30() { return m[3]; }
	
	KReal& m01() { return m[4]; }
	KReal& m11() { return m[5]; }
	KReal& m21() { return m[6]; }
	KReal& m31() { return m[7]; }
	
	KReal& m02() { return m[8]; }
	KReal& m12() { return m[9]; }
	KReal& m22() { return m[10]; }
	KReal& m32() { return m[11]; }
	
	KReal& m03() { return m[12]; }
	KReal& m13() { return m[13]; }
	KReal& m23() { return m[14]; }
	KReal& m33() { return m[15]; }
#else
	KReal m00() const { return m[0]; }
	KReal m01() const { return m[1]; }
	KReal m02() const { return m[2]; }
	KReal m03() const { return m[3]; }

	KReal m10() const { return m[4]; }
	KReal m11() const { return m[5]; }
	KReal m12() const { return m[6]; }
	KReal m13() const { return m[7]; }

	KReal m20() const { return m[8]; }
	KReal m21() const { return m[9]; }
	KReal m22() const { return m[10]; }
	KReal m23() const { return m[11]; }

	KReal m30() const { return m[12]; }
	KReal m31() const { return m[13]; }
	KReal m32() const { return m[14]; }
	KReal m33() const { return m[15]; }


	KReal& m00() { return m[0]; }
	KReal& m01() { return m[1]; }
	KReal& m02() { return m[2]; }
	KReal& m03() { return m[3]; }

	KReal& m10() { return m[4]; }
	KReal& m11() { return m[5]; }
	KReal& m12() { return m[6]; }
	KReal& m13() { return m[7]; }

	KReal& m20() { return m[8]; }
	KReal& m21() { return m[9]; }
	KReal& m22() { return m[10]; }
	KReal& m23() { return m[11]; }

	KReal& m30() { return m[12]; }
	KReal& m31() { return m[13]; }
	KReal& m32() { return m[14]; }
	KReal& m33() { return m[15]; }
#endif

	static const KtMatrix4 ZERO;
	static const KtMatrix4 IDENTITY;

private:
	// 数据成员
	KReal m[16];
};



template<class KReal>
void KtMatrix4<KReal>::MakeTransform(const KtVector3<KReal>& kvPos, const KtVector3<KReal>& kvScale,
	const KtQuaternion<KReal>& kvOrientation)
{
	// Ordering:
	//    1. Scale
	//    2. Rotate
	//    3. Translate

	KtMatrix3<KReal> ktRot3x3;
	kvOrientation.ToRotationMatrix(ktRot3x3);

	// Set up final matrix with scale, rotation and translation
	m00() = kvScale.x * ktRot3x3.m00(); m01() = kvScale.y * ktRot3x3.m01(); m02() = kvScale.z * ktRot3x3.m02(); m03() = kvPos.x;
	m10() = kvScale.x * ktRot3x3.m10(); m11() = kvScale.y * ktRot3x3.m11(); m12() = kvScale.z * ktRot3x3.m12(); m13() = kvPos.y;
	m20() = kvScale.x * ktRot3x3.m20(); m21() = kvScale.y * ktRot3x3.m21(); m22() = kvScale.z * ktRot3x3.m22(); m23() = kvPos.z;

	// No projection term
	m30() = 0; m31() = 0; m32() = 0; m33() = 1;
}

template<class KReal>
bool KtMatrix4<KReal>::operator==(const KtMatrix4<KReal>& rhs) const
{
	return m00() == rhs.m00() && m01() == rhs.m01() && m02() == rhs.m02() && m03() == rhs.m03() &&
		m10() == rhs.m10() && m11() == rhs.m11() && m12() == rhs.m12() && m13() == rhs.m13() &&
		m20() == rhs.m20() && m21() == rhs.m21() && m22() == rhs.m22() && m23() == rhs.m23() &&
		m30() == rhs.m30() && m31() == rhs.m31() && m32() == rhs.m32() && m33() == rhs.m33();
}

template<class KReal>
bool KtMatrix4<KReal>::IsApproxEqual(const KtMatrix4<KReal>& rhs) const
{
	for (int i = 0; i < 16; i++)
	{
		if (!KtuMath<KReal>::ApproxEqual(m[i], rhs[i]))
			return false;
	}

	return true;
}

template<class KReal>
KtMatrix4<KReal>& KtMatrix4<KReal>::operator=(const KtMatrix3<KReal>& rhs)
{
	m00() = rhs.m00();	m01() = rhs.m01();	m02() = rhs.m02();
	m10() = rhs.m10();	m11() = rhs.m11();	m12() = rhs.m12();
	m20() = rhs.m20();	m21() = rhs.m21();	m22() = rhs.m22();

	return *this;
}

template<class KReal>
KtMatrix4<KReal> KtMatrix4<KReal>::operator*(const KtMatrix4<KReal>& rhs) const
{
	KtMatrix4<KReal> r;
	r.m00() = m00() * rhs.m00() + m01() * rhs.m10() + m02() * rhs.m20() + m03() * rhs.m30();
	r.m01() = m00() * rhs.m01() + m01() * rhs.m11() + m02() * rhs.m21() + m03() * rhs.m31();
	r.m02() = m00() * rhs.m02() + m01() * rhs.m12() + m02() * rhs.m22() + m03() * rhs.m32();
	r.m03() = m00() * rhs.m03() + m01() * rhs.m13() + m02() * rhs.m23() + m03() * rhs.m33();

	r.m10() = m10() * rhs.m00() + m11() * rhs.m10() + m12() * rhs.m20() + m13() * rhs.m30();
	r.m11() = m10() * rhs.m01() + m11() * rhs.m11() + m12() * rhs.m21() + m13() * rhs.m31();
	r.m12() = m10() * rhs.m02() + m11() * rhs.m12() + m12() * rhs.m22() + m13() * rhs.m32();
	r.m13() = m10() * rhs.m03() + m11() * rhs.m13() + m12() * rhs.m23() + m13() * rhs.m33();

	r.m20() = m20() * rhs.m00() + m21() * rhs.m10() + m22() * rhs.m20() + m23() * rhs.m30();
	r.m21() = m20() * rhs.m01() + m21() * rhs.m11() + m22() * rhs.m21() + m23() * rhs.m31();
	r.m22() = m20() * rhs.m02() + m21() * rhs.m12() + m22() * rhs.m22() + m23() * rhs.m32();
	r.m23() = m20() * rhs.m03() + m21() * rhs.m13() + m22() * rhs.m23() + m23() * rhs.m33();

	r.m30() = m30() * rhs.m00() + m31() * rhs.m10() + m32() * rhs.m20() + m33() * rhs.m30();
	r.m31() = m30() * rhs.m01() + m31() * rhs.m11() + m32() * rhs.m21() + m33() * rhs.m31();
	r.m32() = m30() * rhs.m02() + m31() * rhs.m12() + m32() * rhs.m22() + m33() * rhs.m32();
	r.m33() = m30() * rhs.m03() + m31() * rhs.m13() + m32() * rhs.m23() + m33() * rhs.m33();

	return r;
}

template<class KReal>
KtVector4<KReal> KtMatrix4<KReal>::operator*(const KtVector4<KReal>& v) const
{
	KtVector4<KReal> r;
	r.x = m00() * v.x + m01() * v.y + m02() * v.z + m03() * v.w;
	r.y = m10() * v.x + m11() * v.y + m12() * v.z + m13() * v.w;
	r.z = m20() * v.x + m21() * v.y + m22() * v.z + m23() * v.w;
	r.w = m30() * v.x + m31() * v.y + m32() * v.z + m33() * v.w;

	return r;
}

template<class KReal>
KtVector4<KReal> KtMatrix4<KReal>::operator*(const KtVector3<KReal>& v) const
{
	KtVector4<KReal> r;
	r.x = m00() * v.x + m01() * v.y + m02() * v.z + m03();
	r.y = m10() * v.x + m11() * v.y + m12() * v.z + m13();
	r.z = m20() * v.x + m21() * v.y + m22() * v.z + m23();
	r.w = m30() * v.x + m31() * v.y + m32() * v.z + m33();

	return r;
}

#if 0
// performs 4x4-matrix inversion with Cramer's Rule
template<class KReal>
void KtMatrix4<KReal>::Inverse0(const KtMatrix4<KReal>& mat)
{
	KReal tmp[12]; /* temp array for pairs */
	KReal src[16]; /* array of transpose source matrix */
	KReal det; /* determinant */
	/* transpose matrix */
	for (int i = 0; i < 4; i++) {
		src[i] = mat[i * 4];
		src[i + 4] = mat[i * 4 + 1];
		src[i + 8] = mat[i * 4 + 2];
		src[i + 12] = mat[i * 4 + 3];
	}
	/* calculate pairs for first 8 elements (cofactors) */
	tmp[0] = src[10] * src[15];
	tmp[1] = src[11] * src[14];
	tmp[2] = src[9] * src[15];
	tmp[3] = src[11] * src[13];
	tmp[4] = src[9] * src[14];
	tmp[5] = src[10] * src[13];
	tmp[6] = src[8] * src[15];
	tmp[7] = src[11] * src[12];
	tmp[8] = src[8] * src[14];
	tmp[9] = src[10] * src[12];
	tmp[10] = src[8] * src[13];
	tmp[11] = src[9] * src[12];
	/* calculate first 8 elements (cofactors) */
	m[0] = tmp[0] * src[5] + tmp[3] * src[6] + tmp[4] * src[7];
	m[0] -= tmp[1] * src[5] + tmp[2] * src[6] + tmp[5] * src[7];
	m[1] = tmp[1] * src[4] + tmp[6] * src[6] + tmp[9] * src[7];
	m[1] -= tmp[0] * src[4] + tmp[7] * src[6] + tmp[8] * src[7];
	m[2] = tmp[2] * src[4] + tmp[7] * src[5] + tmp[10] * src[7];
	m[2] -= tmp[3] * src[4] + tmp[6] * src[5] + tmp[11] * src[7];
	m[3] = tmp[5] * src[4] + tmp[8] * src[5] + tmp[11] * src[6];
	m[3] -= tmp[4] * src[4] + tmp[9] * src[5] + tmp[10] * src[6];
	m[4] = tmp[1] * src[1] + tmp[2] * src[2] + tmp[5] * src[3];
	m[4] -= tmp[0] * src[1] + tmp[3] * src[2] + tmp[4] * src[3];
	m[5] = tmp[0] * src[0] + tmp[7] * src[2] + tmp[8] * src[3];
	m[5] -= tmp[1] * src[0] + tmp[6] * src[2] + tmp[9] * src[3];
	m[6] = tmp[3] * src[0] + tmp[6] * src[1] + tmp[11] * src[3];
	m[6] -= tmp[2] * src[0] + tmp[7] * src[1] + tmp[10] * src[3];
	m[7] = tmp[4] * src[0] + tmp[9] * src[1] + tmp[10] * src[2];
	m[7] -= tmp[5] * src[0] + tmp[8] * src[1] + tmp[11] * src[2];
	/* calculate pairs for second 8 elements (cofactors) */
	tmp[0] = src[2] * src[7];
	tmp[1] = src[3] * src[6];
	tmp[2] = src[1] * src[7];
	tmp[3] = src[3] * src[5];
	tmp[4] = src[1] * src[6];
	tmp[5] = src[2] * src[5];

	tmp[6] = src[0] * src[7];
	tmp[7] = src[3] * src[4];
	tmp[8] = src[0] * src[6];
	tmp[9] = src[2] * src[4];
	tmp[10] = src[0] * src[5];
	tmp[11] = src[1] * src[4];
	/* calculate second 8 elements (cofactors) */
	m[8] = tmp[0] * src[13] + tmp[3] * src[14] + tmp[4] * src[15];
	m[8] -= tmp[1] * src[13] + tmp[2] * src[14] + tmp[5] * src[15];
	m[9] = tmp[1] * src[12] + tmp[6] * src[14] + tmp[9] * src[15];
	m[9] -= tmp[0] * src[12] + tmp[7] * src[14] + tmp[8] * src[15];
	m[10] = tmp[2] * src[12] + tmp[7] * src[13] + tmp[10] * src[15];
	m[10] -= tmp[3] * src[12] + tmp[6] * src[13] + tmp[11] * src[15];
	m[11] = tmp[5] * src[12] + tmp[8] * src[13] + tmp[11] * src[14];
	m[11] -= tmp[4] * src[12] + tmp[9] * src[13] + tmp[10] * src[14];
	m[12] = tmp[2] * src[10] + tmp[5] * src[11] + tmp[1] * src[9];
	m[12] -= tmp[4] * src[11] + tmp[0] * src[9] + tmp[3] * src[10];
	m[13] = tmp[8] * src[11] + tmp[0] * src[8] + tmp[7] * src[10];
	m[13] -= tmp[6] * src[10] + tmp[9] * src[11] + tmp[1] * src[8];
	m[14] = tmp[6] * src[9] + tmp[11] * src[11] + tmp[3] * src[8];
	m[14] -= tmp[10] * src[11] + tmp[2] * src[8] + tmp[7] * src[9];
	m[15] = tmp[10] * src[10] + tmp[4] * src[8] + tmp[9] * src[9];
	m[15] -= tmp[8] * src[9] + tmp[11] * src[10] + tmp[5] * src[8];
	/* calculate determinant */
	det = src[0] * m[0] + src[1] * m[1] + src[2] * m[2] + src[3] * m[3];
	/* calculate matrix inverse */
	det = 1 / det;
	for (int j = 0; j < 16; j++)
		m[j] *= det;
}
#endif

template<class KReal>
KtMatrix4<KReal> KtMatrix4<KReal>::GetInverse() const
{
	KtMatrix4<KReal> inverseMat;
	inverseMat.SetInverseOf(*this);
	return inverseMat;
}

template<class KReal>
KtMatrix4<KReal>& KtMatrix4<KReal>::Inverse()
{
	return *this = GetInverse();
}

// OGRE版本，比Inverse0要快很多，至少是几十倍
template<class KReal>
KtMatrix4<KReal>& KtMatrix4<KReal>::SetInverseOf(const KtMatrix4<KReal>& mat)
{
	KReal _00 = mat.m00(), _01 = mat.m01(), _02 = mat.m02(), _03 = mat.m03();
	KReal _10 = mat.m10(), _11 = mat.m11(), _12 = mat.m12(), _13 = mat.m13();
	KReal _20 = mat.m20(), _21 = mat.m21(), _22 = mat.m22(), _23 = mat.m23();
	KReal _30 = mat.m30(), _31 = mat.m31(), _32 = mat.m32(), _33 = mat.m33();

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

using mat4f = KtMatrix4<float>;
using mat4d = KtMatrix4<double>;

using float4x4 = mat4f;
using double4x4 = mat4d;
