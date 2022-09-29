#pragma once
#include "KtPoint.h"
#include <assert.h>


/** A 3D box aligned with the x/y/z axes.
@remarks
This class represents a simple box which is aligned with the
axes. Internally it only stores 2 points as the extremeties of
the box, one which is the minima of all 3 axes, and the rhs
which is the maxima of all 3 axes. 
*/
template<class T>
class KtAABB
{
	using point = KtPoint<T, 3>;
	using kMath = KtuMath<T>;

public:

	/*
		1-----2
	   /|    /|
	  / |   / |
	 5-----4  |
	 |  0--|--3
	 | /   | /
	 |/    |/
	 6-----7
	*/
	enum KeBoxCorner
	{
		k_far_left_bottom,
		k_far_left_top,
		k_far_right_top,
		k_far_right_bottom,
		k_near_right_top,
		k_near_left_top,
		k_near_left_bottom,
		k_near_right_bottom
	};

	// Default to a null box 
	KtAABB() { setNull(); }

	KtAABB(const KtAABB& box)
		: lower_(box.lower_), upper_(box.upper_) {}

	KtAABB(const point& lower, const point& upper)
		: lower_(lower), upper_(upper) {}

	KtAABB(T mx, T my, T mz, T Mx, T My, T Mz)
		: lower_(mx, my, mz), upper_(Mx, My, Mz) {}


	KtAABB& operator=(const KtAABB& rhs) {
		// Specifically override to avoid copying m_pkCorners
		lower_ = rhs.lower_;
		upper_ = rhs.upper_;
		return *this;
	}


	/** Gets the minimum corner of the box.
	*/
	const point& lower() const { return lower_; }

	/** Gets a modifiable version of the minimum
	corner of the box.
	*/
	point& lower() { return lower_; }


	/** Gets the maximum corner of the box.
	*/
	const point& upper() const { return upper_; }

	/** Gets a modifiable version of the maximum
	corner of the box.
	*/
	point& upper() { return upper_; }


	/** Sets both minimum and maximum extents at once.
	*/
	void setExtents(const point& lower, const point& upper) {
		assert(lower.x() <= upper.x() && lower.y() <= upper.y() && lower.z() <= upper.z());
		lower_ = lower, upper_ = upper;
	}

	void setExtents(T mx, T my, T mz, T Mx, T My, T Mz) {
		assert(mx <= Mx && my <= My && mz <= Mz);
		lower_ = { mx, my, mz };
		upper_ = { Mx, My, Mz };
	}

	/** gets the position of one of the corners
	*/
	point corner(KeBoxCorner id) const;

	/** Returns an array of 8 corner points, useful for
	collision vs. non-aligned objects.
	*/
	std::vector<point> allCorners() const {
		std::vector<point> pts(8);
		for (int i = 0; i < 8; i++)
			pts[i] = corner(KeBoxCorner(i));
		return pts;
	}


	/// Gets the centre of the box
	point center() const {
		return isNull() ? point(0) : (lower_ + upper_) / 2;
	}

	/// Gets the size of the box
	point size() const {
		return isNull() ? point(0) : upper_ - lower_;
	}

	/// Calculate the volume of this box
	T volume() const {
		auto sz = size();
		return sz.x() * sz.y() * sz.z();
	}

	/** Sets the box to a 'null' value i.e. not a box.
	*/
	void setNull() {
		lower_ = upper_ = point(kMath::nan);
	}

	/** Returns true if the box is null i.e. empty.
	*/
	bool isNull() const {
		return lower_.isNan() || upper_.isNan();
	}

	/** Sets the box to 'infinite'
	*/
	void setInf() {
		setExtents(point(-kMath::inf), point(kMath::inf));
	}

	/** Returns true if the box is infinite.
	*/
	bool isInf() const {
		return lower_.isInf() || upper_.isInf();
	}

	/** Returns true if the box is finite.
	*/
	bool isFinite() const {
		return !isNull() && !isInf();
	}

	/** Tests 2 boxes for equality.
	*/
	bool operator ==(const KtAABB& rhs) const {
		return lower_ == rhs.lower_ && upper_ == rhs.upper_;
	}

	/** Tests 2 boxes for inequality.
	*/
	bool operator !=(const KtAABB& rhs) const {
		return lower_ != rhs.lower_ || upper_ != rhs.upper_;
	}

	/** Tests whether the given point contained by this box.
	*/
	bool contains(const point& v) const;

	/** Tests whether another box contained by this box.
	*/
	bool contains(const KtAABB& rhs) const;

	/** Scales the AABB by the vector given. */
	void scale(const point& v) {
		// NB assumes centered on origin
		lower_ *= v, upper_ *= v;
	}


	/** Merges the passed in box into the current box. The result is the
	box which encompasses both.
	*/
	KtAABB& merge(const KtAABB& rhs);

	/** Extends the box to encompass the specified point (if needed).
	*/
	KtAABB& merge(const point& pt);

	/// Calculate the area of intersection of this box and another
	KtAABB intersection(const KtAABB& rhs) const;

	/** Returns whether or not this box intersects another. */
	bool intersects(const KtAABB& rhs) const;

private:
	KtPoint<T, 3> lower_, upper_;
};

template<class T>
KtPoint<T, 3> KtAABB<T>::corner(KeBoxCorner id) const 
{
	if (isNull())
		return point(0);

	switch (id)
	{
	case k_far_left_bottom:
		return lower_;

	case k_far_left_top:
		return { lower_.x(), upper_.y(), lower_.z() };

	case k_far_right_top:
		return { upper_.x(), upper_.y(), lower_.z() };

	case k_far_right_bottom:
		return { upper_.x(), lower_.y(), lower_.z() };

	case k_near_right_bottom:
		return { upper_.x(), lower_.y(), upper_.z() };

	case k_near_left_bottom:
		return { lower_.x(), lower_.y(), upper_.z() };

	case k_near_left_top:
		return { lower_.x(), upper_.y(), upper_.z() };

	case k_near_right_top:
		return upper_;

	default:
		assert(false);
	}

	return { 0 }; // make compiler happy
}

template<class T>
bool KtAABB<T>::contains(const point& v) const 
{
	if (isNull())
		return false;
	else if (isInf())
		return true;

	return lower_.x() <= v.x() && v.x() <= upper_.x()
		&& lower_.y() <= v.y() && v.y() <= upper_.y()
		&& lower_.z() <= v.z() && v.z() <= upper_.z();
}

template<class T>
bool KtAABB<T>::contains(const KtAABB& rhs) const 
{
	if (isInf() || rhs.isNull())
		return true;

	if (isNull())
		return false;

	return lower_.x() <= rhs.lower_.x()
		&& lower_.y() <= rhs.lower_.y()
		&& lower_.z() <= rhs.lower_.z()
		&& rhs.upper_.x() <= upper_.x()
		&& rhs.upper_.y() <= upper_.y()
		&& rhs.upper_.z() <= upper_.z();
}

template<class T>
KtAABB<T>& KtAABB<T>::merge(const point& pt)
{
	if (isNull()) // if null, use this point
		setExtents(pt, pt);
	else if (isFinite()) {
		auto lower = point::floor(lower_, pt);
		auto upper = point::ceil(upper_, pt);
		setExtents(lower, upper);
	}

	return *this;
}

template<class T>
KtAABB<T>& KtAABB<T>::merge(const KtAABB& rhs)
{
	// Do nothing if rhs null, or this is infinite
	if (rhs.isNull() || isInf())
		return *this;
	// Otherwise if rhs is infinite, make this infinite, too
	else if (rhs.isInf())
		setInf();
	// Otherwise if current null, just take rhs
	else if (isNull())
		*this = rhs;
	// Otherwise merge
	else {
		auto lower = point::floor(lower_, rhs.lower_);
		auto upper = point::ceil(upper_, rhs.upper_);
		setExtents(lower, upper);
	}

	return *this;
}

template<class T>
KtAABB<T> KtAABB<T>::intersection(const KtAABB& rhs) const
{
	if (isNull() || rhs.isNull())
		return KtAABB();
	else if (isInf())
		return rhs;
	else if (rhs.isInf())
		return *this;

	auto lower = point::ceil(lower_, rhs.lower_);
	auto upper = point::floor(upper_, rhs.upper_);

	// Check intersection isn't null
	if (lower.x() < upper.x() && lower.y() < upper.y() && lower.z() < upper.z())
		return KtAABB(lower, upper);

	return KtAABB();
}

template<class T>
bool KtAABB<T>::intersects(const KtAABB& rhs) const
{
	// Early-fail for nulls
	if (isNull() || rhs.isNull())
		return false;

	// Early-success for infinites
	if (isInf() || rhs.isInf())
		return true;

	// Use up to 6 separating planes
	if (upper_.x() < rhs.lower_.x())
		return false;
	if (upper_.y() < rhs.lower_.y())
		return false;
	if (upper_.z() < rhs.lower_.z())
		return false;

	if (lower_.x() > rhs.upper_.x())
		return false;
	if (lower_.y() > rhs.upper_.y())
		return false;
	if (lower_.z() > rhs.upper_.z())
		return false;

	// otherwise, must be intersecting
	return true;
}