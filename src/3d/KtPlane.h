#pragma once
#include "KtVector3.h"
#include "KtAABB.h"


/** Defines a plane in 3D space.
    @remarks
        A plane is defined in 3D space by the equation
        Ax + By + Cz + D = 0
    @par
        This equates to a vector (the normal of the plane, whose x, y
        and z components equate to the coefficients A, B and C
        respectively), and a constant (D) which is the distance along
        the normal you have to go to move the plane back to the origin.
    */
template<class KReal>
class KtPlane
{
	using vec3 = KtVector3<KReal>;
	using mat3 = KtMatrix3<KReal>;

public:

	/** The "positive side" of the plane is the half space to which the
		plane normal points. The "negative side" is the other half
		space. The flag "no side" indicates the plane itself.
	*/
	enum KeSide
	{
		k_no_side,
		k_postive_side,
		k_negative_side,
		k_both_side
	};

	/** Default constructor - sets everything to 0.
	*/
	KtPlane();

	// 直接指定平面方程的4个参数
	KtPlane(KReal a, KReal b, KReal c, KReal d);

	/** Construct a plane through a normal, and a distance to move the plane along the normal.*/
	KtPlane(const vec3& normal, KReal c);

	// 3点成面
	KtPlane(const vec3& pt0, const vec3& pt1, const vec3& pt2);

	KtPlane(const vec3& normal, const vec3& pt);

	KtPlane(const KtPlane& rhs);


	KeSide getSide(const vec3& pt) const;

	/**
	Returns the side where the alignedBox is. The flag BOTH_SIDE indicates an intersecting box.
	One corner ON the plane is sufficient to consider the box and the plane intersecting.
	*/
	KeSide getSide(const KtAABB<KReal>& box) const;

	/** Returns which side of the plane that the given box lies on.
		The box is defined as centre/half-size pairs for effectively.
	@param centre The centre of the box.
	@param halfSize The half-size of the box.
	@return
		POSITIVE_SIDE if the box complete lies on the "positive side" of the plane,
		NEGATIVE_SIDE if the box complete lies on the "negative side" of the plane,
		and BOTH_SIDE if the box intersects the plane.
	*/
	KeSide getSide(const vec3& centre, const vec3& halfSize) const;

	/** This is a pseudodistance. The sign of the return value is
		positive if the point is on the positive side of the plane,
		negative if the point is on the negative side, and zero if the
		point is on the plane.
		@par
		The absolute value of the return value is the true distance only
		when the plane normal is a unit length vector.
	*/
	KReal distance(const vec3& pt) const;

	/** Project a vector onto the plane. 
	@remarks This gives you the element of the input vector that is perpendicular 
		to the normal of the plane. You can get the element which is parallel
		to the normal of the plane by subtracting the result of this method
		from the original vector, since parallel + perpendicular = original.
	@param v The input vector
	*/
	vec3 projectVector(const vec3& v) const;

	/** Normalises the plane.
		@remarks
			This method normalises the plane's normal and the length scale of d
			is as well.
		@note
			This function will not crash for zero-sized vectors, but there
			will be no changes made to their components.
		@return The previous length of the plane's normal.
	*/
	void normalize();

	/// Comparison operator
	bool operator==(const KtPlane& rhs) const {
		return (rhs.d == d && rhs.normal == normal);
	}

	bool operator!=(const KtPlane& rhs) const {
		return (rhs.d != d || rhs.normal != normal);
	}

	// accessor
	const vec3& normal() const{ return normal; }
	KReal distance() const { return d_; }


private:
	vec3 normal_;
	KReal d_;
};


//-----------------------------------------------------------------------
template<class KReal>
KtPlane<KReal>::KtPlane<KReal>()
	: normal_{ 0 }, d_{ 0 }
{

}

//-----------------------------------------------------------------------
template<class KReal>
KtPlane<KReal>::KtPlane<KReal>(const KtPlane<KReal>& rhs)
	: normal_(rhs.normal_), d_(rhs.d_)
{

}

//-----------------------------------------------------------------------
template<class KReal>
KtPlane<KReal>::KtPlane<KReal>(const vec3& normal, KReal c)
    : normal_(normal), d_(-c)
{

}

//---------------------------------------------------------------------
template<class KReal>
KtPlane<KReal>::KtPlane<KReal>(KReal a, KReal b, KReal c, KReal d)
	: normal_(a, b, c), d_(d)
{
}

//-----------------------------------------------------------------------
template<class KReal>
KtPlane<KReal>::KtPlane<KReal>(const vec3& normal, const vec3& pt)
	: normal_(normal)
{
	d_ = -normal.dot(pt);
}

//-----------------------------------------------------------------------
template<class KReal>
KtPlane<KReal>::KtPlane<KReal>(const vec3& pt0, const vec3& pt1, const vec3& pt2)
{
	auto e1 = pt1 - pt0;
	auto e2 = pt2 - pt0;
	normal_ = e1.corss(e2);
	normal_.normalize();
	d_ = -normal_.dot(pt0);
}

//-----------------------------------------------------------------------
template<class KReal>
KReal KtPlane<KReal>::distance(const vec3& pt) const
{
	return normal_.dot(pt) + d_;
}

//-----------------------------------------------------------------------
template<class KReal>
KtPlane<KReal>::KeSide KtPlane<KReal>::getSide(const vec3& pt) const
{
	KReal dist = distance(pt);

	if (dist < 0.0)
		return k_negative_side;

	if (dist > 0.0)
		return k_postive_side;

	return k_no_side;
}

//-----------------------------------------------------------------------
template<class KReal>
KtPlane<KReal>::KeSide KtPlane<KReal>::getSide(const KtAABB<KReal>& box) const
{
	if (box.isNull())
		return k_no_side;
	if (box.isInfinite())
		return k_both_side;

	return getSide(box.center(), box.size() / 2);
}

//-----------------------------------------------------------------------
template<class KReal>
KtPlane<KReal>::KeSide KtPlane<KReal>::getSide(const vec3& centre, const vec3& halfSize) const
{
	// Calculate the distance between box centre and the KtPlane
	KReal dist = distance(centre);

	// Calculate the maximise allows absolute distance for
	// the distance between box centre and KtPlane
	KReal maxAbsDist = normal.absDot(halfSize);

	if (dist < -maxAbsDist)
		return k_negative_side;

	if (dist > +maxAbsDist)
		return k_postive_side;

	return k_both_side;
}

//-----------------------------------------------------------------------
template<class KReal>
KtVector3<KReal> KtPlane<KReal>::projectVector(const vec3& v) const
{
	// We know KtPlane normal is unit length, so use simple method
	mat3 xform(1.0f - normal_.x() * normal_.x(),
		       -normal_.x() * normal_.y(),
		       -normal_.x() * normal_.z(),

		       -normal_.y() * normal_.x(),
		       1.0f - normal_.y() * normal_.y(),
		       -normal_.y() * normal_.z(),

		       -normal_.z() * normal_.x(),
		       -normal_.z() * normal_.y(),
		       1.0f - normal_.z() * normal_.z());

	return xform * p;

}

//-----------------------------------------------------------------------
template<class KReal>
void KtPlane<KReal>::normalize(void)
{
	KReal fLength = normal_.length();

	if (fLength > KReal(0.0f))
	{
		KReal fInvLength = 1.0f / fLength;
		normal_ *= fInvLength;
		d_ *= fInvLength;
	}
}